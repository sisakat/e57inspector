/*
Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2023 Stefan Isak <http://sisak.at>.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef SILRUCACHE_H
#define SILRUCACHE_H

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <stdexcept>

template <typename Value, typename Size = uint32_t, Size defaultSize = 1>
class SiLRUCacheItem {
public:
    SiLRUCacheItem() 
    {}

    SiLRUCacheItem(Value&& val) 
        : m_value{std::move(val)} 
    {}

    SiLRUCacheItem(const Value& val) 
        : m_value{val}
    {}

    SiLRUCacheItem(Value&& val, Size size) 
        : m_value{val}, m_size{size}
    {}

    SiLRUCacheItem(const Value& val, Size size) 
        : m_value{val}, m_size{size}
    {}

    Size size() const {
        return m_size;
    }

    Value& value() {
        return m_value;
    }

    const Value& value() const {
        return m_value;
    }

    Value& operator*() {
        return m_value;
    }

    const Value& operator*() const {
        return m_value;
    }

private:
    Size m_size{defaultSize};
    Value m_value{};
};

template <typename Key, typename Value, typename Size = uint32_t, Size defaultSize = 1>
class SiLRUCache {
public:
    using CacheItemType = SiLRUCacheItem<Value, Size, defaultSize>;

    SiLRUCache(Size cacheSize)
        : m_cacheSize(cacheSize), m_queue(0)
    {}

    /**
     * @brief Adds a new item into the cache.
     * 
     * @param key Cache Key
     * @param value Cache Value
     */
    void addItem(const Key& key, const Value& value) {
        checkQueueSize(defaultSize);
        m_cacheItems.emplace(key, value);
        updateQueue(key);
    }

    /**
     * @brief Adds a new item into the cache.
     * 
     * @param key Cache Key
     * @param value Cache Value
     */
    void addItem(const Key& key, Value&& value) {
        checkQueueSize(defaultSize);
        m_cacheItems.emplace(key, std::move(value));
        updateQueue(key);
    }

    /**
     * @brief Adds a new item into the cache.
     * 
     * @param key Cache Key
     * @param value Cache Value
     */
    void addItem(const Key& key, const CacheItemType& value) {
        checkQueueSize(value.size());
        m_cacheItems.emplace(key, value);
        updateQueue(key);
    }

    /**
     * @brief Adds a new item into the cache.
     * 
     * @param key Cache Key
     * @param value Cache Value
     */
    void addItem(const Key& key, CacheItemType&& value) {
        checkQueueSize(value.size());
        m_cacheItems.emplace(key, std::move(value));
        updateQueue(key);
    }

    /**
     * @brief Removes the cache item with the specified key from the cache.
     * 
     * @param key Cache Key
     */
    void removeItem(const Key& key) {
        if (!contains(key)) return;
        m_cacheItems.erase(key);
        removeFromQueue(key);
    }

    /**
     * @brief Checks if the item is contained in the cache.
     * 
     * @param key Key of the cached item to look for.
     * @return true If the key is contained in the cache.
     * @return false If the key is not contained in the cache.
     */
    bool contains(const Key& key) {
        return m_cacheItems.find(key) != m_cacheItems.end();
    }

    /**
     * @brief Retrieve the item from the cache.
     * 
     * @param key Key of the cache item.
     * @return Value* Pointer to the value.
     */
    auto& getItem(const Key& key) {
        updateQueue(key);
        return m_cacheItems.at(key);
    }

    /**
     * @brief Retrieve the item from the cache.
     * 
     * @param key Key of the cache item.
     * @return Value* Pointer to the value.
     */
    const auto& getItem(const Key& key) const {
        updateQueue(key);
        return m_cacheItems.at(key);
    }

    /**
     * @brief Updates the least-recently-used tracker for that specific cache item.
     * 
     * @param key Key of the cache item.
     */
    void update(const Key& key) {
        updateQueue(key);
    }

    /**
     * @brief Reset the cache. Destroys all cache items.
     * 
     */
    void reset() {
        m_queue.clear();
        m_cacheItems.clear();
    }

    /**
     * @brief Returns the total size of all elements inside the cache.
     * 
     * @return Size 
     */
    Size size() const {
        Size totalSize{};
        for (const auto& [key, value] : m_cacheItems) {
            totalSize += value.size();
        }
        return totalSize;
    }

private:
    const Size m_cacheSize;
    std::vector<Key> m_queue;
    std::unordered_map<Key, CacheItemType> m_cacheItems;

    void updateQueue(const Key& key) {
        if (m_queue.size() > 0 && m_queue.back() == key) return;
        removeFromQueue(key);
        m_queue.push_back(key);
    }

    void checkQueueSize(Size requestedSize) {
        while (size() + requestedSize > m_cacheSize) {
            if  (m_queue.empty()) {
                throw std::runtime_error("Cache too small.");
            }
            m_cacheItems.erase(m_queue.front());
            m_queue.erase(m_queue.begin(), m_queue.begin() + 1);
        }
    }

    void removeFromQueue(const Key& key) {
        std::remove_if(m_queue.begin(), m_queue.end(), [&key](const auto& k) {
            return k == key;
        });
    }
};

#endif
