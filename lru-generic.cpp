
#include <list>
#include <map>
#include <iostream>
#include <unordered_map>
#include <pthread.h>

/**
 *
 * - 通用存储
 * - 线程安全
 * - O(1)存取复杂度
 *
 *
 * @tparam K
 * @tparam V
 */
template<typename K, typename V>
class LRUCache
{
private:
    typedef K key_t;
    typedef V value_t;
    typedef struct
    {
        key_t key;
        value_t value;
    } Node_t;
    typedef std::list<Node_t> cacheList_t;
    typedef std::unordered_map<key_t, typename cacheList_t::iterator> map_t;

    int m_capacity;             // 缓存容量
    cacheList_t m_cacheList;    // node 链表
    map_t m_mp;
    int _hit = 0, _miss = 0;    // 命中率计算
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

public:
    explicit LRUCache(int capacity)
        : m_capacity(capacity)
    {}
    bool get(const K &key, V &value);
    bool put(const K &key, const V &value);
    size_t size() const;
    void clear();
    float hitRate() const;

};

template<typename K, typename V>
bool LRUCache<K, V>::get(const K &key, V &value)
{
    pthread_mutex_lock(&lock);

    auto it = m_mp.find(key);
    // 未命中,返回 NULL
    if (it == m_mp.end()) {
        _miss++;
        pthread_mutex_unlock(&lock);
        return false;
    }
    _hit++;

    // 命中, 则把链表中此元素删除并放置最前 再返回
    auto list_it = m_mp[key];
    Node_t node = {key, list_it->value};
    m_cacheList.erase(list_it);
    m_cacheList.push_front(node);
    m_mp[key] = m_cacheList.begin();
    value = m_cacheList.begin()->value;

    pthread_mutex_unlock(&lock);
    return true;

}

template<typename K, typename V>
bool LRUCache<K, V>::put(const K &key, const V &value)
{
    pthread_mutex_lock(&lock);

    auto it = m_mp.find(key);
    // cached
    if (it != m_mp.end()) {
        auto listIt = m_mp[key];
        // delete the cached node, and then insert it to the list head
        Node_t node = {key, value};
        m_cacheList.erase(listIt);
        m_cacheList.push_front(node);
        m_mp[key] = m_cacheList.begin();
    }
    else {
        if (m_cacheList.size() == m_capacity) { // 容量满,剔除最早
            m_mp.erase(m_cacheList.back().key);
            m_cacheList.pop_back();
        }
        // 未满, 在链表头插入, 并更新 map 指针
        Node_t node = {key, value};
        m_cacheList.push_front(node);
        m_mp[key] = m_cacheList.begin();
    }
    pthread_mutex_unlock(&lock);
    return true;
}

template<typename K, typename V>
size_t LRUCache<K, V>::size() const
{
    return m_mp.size();
}

template<typename K, typename V>
void LRUCache<K, V>::clear()
{
    m_mp.clear();
    m_cacheList.clear();
    _hit = 0;
    _miss = 0;
}

template<typename K, typename V>
float LRUCache<K, V>::hitRate() const
{
    return _hit / (_hit + _miss);
}



