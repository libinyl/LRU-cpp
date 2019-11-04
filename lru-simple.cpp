
#include <list>
#include <map>
#include <iostream>

using namespace std;


/**
 *
 * LRU 实现:
 * 0. map 容量大小有限
 * 1. get: 未命中, return -1; 命中, 刷新位置
 * 2. put: 存在, 刷新位置; 不存在, 若容量满则剔除末尾元素; 最终添加元素至最前, map 指针指向它
 * 2. 始终保证最近读写的元素为最新(位于队列最前)
 *
 *
 */
class LRUCache
{
private:
    typedef int key_t;
    typedef int value_t;
    typedef struct
    {
        key_t key;
        value_t value;
    } Node_t;
    typedef list<Node_t> cacheList_t;
    typedef map<key_t, cacheList_t::iterator> map_t;

    int m_capacity;
    cacheList_t m_cacheList;
    map_t m_mp;


public:
    LRUCache(int capacity)
        : m_capacity(capacity)
    {}

    int get(int key)
    {
        auto it = m_mp.find(key);
        // 未命中,返回-1
        if (it == m_mp.end())
            return -1;
        else {// 命中, 刷新元素位置,返回
            auto list_it = m_mp[key];
            Node_t node = {key, list_it->value};
            m_cacheList.erase(list_it);
            m_cacheList.push_front(node);
            m_mp[key] = m_cacheList.begin();
            return m_cacheList.begin()->value;
        }
    }

    void put(int key, int value)
    {
        auto it = m_mp.find(key);
        // 已存在,则仅刷新位置
        if (it != m_mp.end()) {
            auto listIt = m_mp[key];
            Node_t node = {key, value};
            m_cacheList.erase(listIt);
            m_cacheList.push_front(node);
            m_mp[key] = m_cacheList.begin();

        }
        else {// 若不存在,则考察容量
            if (m_cacheList.size() == m_capacity) { // 容量满,剔除最旧元素
                m_mp.erase(m_cacheList.back().key);
                m_cacheList.pop_back();
            }
            // 最终在链表头插如新元素, 并令 map 指针指向它
            Node_t node = {key, value};
            m_cacheList.push_front(node);
            m_mp[key] = m_cacheList.begin();
        }
    }
};

int main()
{
    // 简单测试:
    LRUCache cache(3);
    // 不存在元素,返回-1
    cache.get(1);       // returns 1

    // 超过容量,返回-1
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);
    cache.put(4, 4);

    cache.get(1);//return -1

}

