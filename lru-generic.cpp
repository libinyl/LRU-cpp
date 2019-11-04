
#include <list>
#include <map>
#include <iostream>
#include <pthread.h>

using namespace std;


/**
 *
 * 通用存储/线程安全
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
    typedef list<Node_t> cacheList_t;
    typedef map<key_t, typename cacheList_t::iterator> map_t;

    int m_capacity;             // 缓存容量
    cacheList_t m_cacheList;    // node 链表
    map_t m_mp;
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

public:
    explicit LRUCache(int capacity)
        : m_capacity(capacity)
    {}
    bool get(const K &key, V &value);
    bool put(const K &key, const V &value);

};

template<typename K, typename V>
bool LRUCache<K, V>::get(const K &key, V &value)
{
    pthread_mutex_lock(&lock);

    auto it = m_mp.find(key);
    // 未命中,返回 NULL
    if (it == m_mp.end()) {
        pthread_mutex_unlock(&lock);
        return false;
    }

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

