#include <iostream>
#include <vector>
using namespace std;

namespace CloseHash {

    enum State {   // 每个哈希值有三种状态：空、存在、删除
        EMPTY, 
        EXIST,
        DELETE,
    };

    template <typename K, typename V>
    struct HashData {
        std::pair<K, V> _kv;    // 哈希数据
        State _state = EMPTY;   // 数据状态
    };

    // 默认支持整型
    template <typename K>
    struct Hash {
        size_t operator()(const K& key) {
            return key;
        }
    };

    // 对 string 类型的特化
    template <>
    struct Hash<std::string> {
        size_t operator()(const std::string& s) {
            size_t value = 0;
            for (auto e : s) {
                value += e;
                value *= 131;
            }
            return value;
        }
    };

    template <class K, class V, class HashFunc = Hash<K>>
    class HashTable {
    public:
        bool Insert(const std::pair<K, V>& kv) {
            HashData<K, V>* ret = Find(kv.first);
            if (ret) {   // 这个 key 有值
                return false;
            }

            if (_table.size() == 0) {
                _table.resize(10);   // 扩容
            } else if ((double)_n / (double)_table.size() > 0.7) {   // 负载因子大于 0.7， 扩容
                HashTable<K, V, HashFunc> newHashTable;
                newHashTable._table.resize(2 * _table.size());

                for (auto& e : _table) {
                    if (e._state == EXIST) {
                        newHashTable.Insert(e._kv);   // 把老的哈希表的数据都复制到新的哈希表中
                    }
                }
                _table.swap(newHashTable._table);
            }

            HashFunc hf;
            size_t start = hf(kv.first) % _table.size();
            size_t index = start;

            size_t i = 1;  // 线性探测
            while(_table[index]._state == EXIST) {   // 发生冲突，向后探测
                index = start + i;
                index %= _table.size();
                ++i;
            }

            _table[index]._kv = kv;   // 找到空位置就插入
            _table[index]._state = EXIST;
            ++_n;

            return true;
        }

        // 用 k 查找
        HashData<K, V>* Find(const K& key) {
            if (_table.size() == 0) {
                return nullptr;
            }

            HashFunc hf;  // 仿函数
            size_t start = hf(key) % _table.size();   // 除留余数法
            size_t index = start;
            size_t i = 1;
            while(_table[index]._state != EMPTY) {
                if (_table[index]._state == EXIST && _table[index]._kv.first == key) {
                    // 找到了
                    return &_table[index];
                }

                // 冲突时，向后查找
                index = start + i;  // 线性探测， 二次查找时可用 index = start + i*i
                index %= _table.size();
                ++i;
            }
            return nullptr;
        }

        // 删除
        bool Erase(const K& key) {
            HashData<K, V>* ret = Find(key);
            if (ret = nullptr) {
                return false;
            } else {
                ret -> _state = DELETE;
                --_n;

                return false;
            }
        }
    private:
        std::vector<HashData<K, V>> _table;  // 哈希表
        size_t _n = 0;     // 有效数据个数
    };

    void test_CloseHashInt() {
        int a[] = {6, 201, 10, 69, 91, 232};
        HashTable<int, int> ht;

        for (auto e : a) {
            ht.Insert(std::make_pair(e, e));
        }
    }

    void test_CloseHashString() {
        std::string s[] = {"篮球", "足球", "乒乓球", "篮球", "羽毛球"};
        HashTable<std::string, int> ht;

        for (auto e : s) {
            auto ret = ht.Find(e);

            if (ret) {
                ret->_kv.second++;
            } else {
                ht.Insert(std::make_pair(e, 1));
            }
        }
    }
}

// 开散列
namespace OpenHash {

    template <class K>
    struct Hash {
        size_t operator()(const K& key) {
            return key;
        }
    };

    template <>
    struct Hash <std::string> {
        size_t operator()(const std::string& s) {
            size_t value = 0;
            for (auto ch : s) {
                value += ch;
                value *= 131;
            }
            return value;
        }
    };

    // 哈希桶，哈希桶中包含：哈希数据、指向下一个桶的指针
    template <class K, class V>
    struct HashNode {
        HashNode<K, V>* _next;
        std::pair<K, V> _kv;

        HashNode(const std::pair<K, V>& kv) : _next(nullptr), _kv(kv) {}
    };

    // 哈希表
    template <class K, class V, class HashFunc = Hash<K>>
    class HashTable {
        typedef HashNode<K, V> Node;
    public:

        //获取质数
		size_t GetNextPrime(size_t prime)
		{
			const int PRIMECOUNT = 28;
			static const size_t primeList[PRIMECOUNT] =
			{
				53ul, 97ul, 193ul, 389ul, 769ul,
				1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
				49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
				1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
				50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
				1610612741ul, 3221225473ul, 4294967291ul
			};

            size_t i = 0;
            for (; i < PRIMECOUNT; ++i) {
                if (primeList[i] > prime) {
                    return primeList[i];
                }
            }
            return primeList[i];
        }

        // 插入
        bool Insert(const std::pair<K, V>& kv) {
            if (Find(kv)) return false;   // 已存在

            HashFunc hf;
            if (_n == _table.size()) {  // 负载因子等于1，需要扩容
                std::vector<Node*> newHashTable;
                newHashTable.resize(GetNextPrime(_table.size()));

                //遍历旧表的所有节点，重新挂到新表中，可能节点映射的位置也发生了变化
                for (size_t i = 0; i < _table.size(); ++i) {
                    if (_table[i]) {
                        Node* cur = _table[i];   // 哈希桶
                        while (cur) {   // 哈希桶下面的节点
                            Node* next = cur->_next;
                            size_t index = hf(cur->_kv.first) % newHashTable.size();

                            //由于是头插,因此将旧表_table的每个桶的_next都置为新表计算的新位置的第一个桶，将新表的newHashTable[index]置为cur
							cur->_next = newHashTable[index];
							newHashTable[index] = cur;
							cur = next;
                        }
                        _table[i] = nullptr;
                    }
                }
                _table.swap(newHashTable);
            }
            size_t index = hf(kv.first) % _table.size();
			Node* newNode = new Node(kv);
 
			//不需要增容，直接头插
			newNode->_next = _table[index];
			_table[index] = newNode;
			_n++;
 
			return true;
        }

        // 查找
        Node* Find(const K& key) {
            if (_table.size() == 0) return nullptr;

            HashFunc hf;
            size_t index = hf(key) % _table.size();
            Node* cur = _table[index];
            while (cur) {
                if (cur->_kv.first == key) {
                    return cur;
                } else {
                    cur = cur->_next;
                }
            }

            return nullptr;
        }

        //删除
		bool Erase(const K& key)
		{
			size_t index = hf(key) % _table.size();
			Node* prev = nullptr;
			Node* cur = _table[index];
 
			while (cur)
			{
				if (cur->_kv.first == key)
				{
					if (_table[index] == cur)//要删除的key就是该位置的第一个桶
					{
						_table[index] = cur->_next;
					}
					else
					{
						prev->_next = cur->_next;
					}
					
					--_n;
					delete cur;
					return true;
 
				}
 
				prev = cur;
				cur = cur->next;
 
				return false;
			}
		}

    private:
        std::vector<Node*> _table;   // 哈希表
        size_t _n = 0;               // 有效数据个数
    };
}

int main() {
    CloseHash::test_CloseHashInt();
    CloseHash::test_CloseHashString();
}