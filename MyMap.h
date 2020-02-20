#ifndef MyMap_h
#define MyMap_h
// MyMap.h
// Skeleton for the MyMap class template.  You must implement the first six
// member functions.

#include "support.h"

template<typename KeyType, typename ValueType>
class MyMap
{
public:
    MyMap()
    {
        root = nullptr;
        m_size = 0;
    }
    ~MyMap()
    {
        clear();
        m_size = 0;
    }
    void clear()
    {
        clear(root);
    }
    void inorder()
    {
        inorder(root);
    }
    int size() const
    {
        return m_size;
    }
    void associate(const KeyType& key, const ValueType& value)
    {
        node* c = findAppear(key); //find the pointer pointing to the ValueType
        if (c == nullptr)
        {
            m_size++;
            if (root == nullptr)
            {
                root = new node(key, value); //create a new node at root if the tree is empty
                return;
            }
            node* cur = root;
            for (;;) //find the appropriate position to insert the new node
            {
                if(key == cur->kValue)
                    return;
                if (key < cur->kValue)
                {
                    if(cur->left != nullptr)
                        cur = cur->left;
                    else
                    {
                        cur->left = new node(key, value);
                        return;
                    }
                }
                else
                {
                    if(cur->right != nullptr)
                        cur = cur->right;
                    else
                    {
                        cur->right = new node(key, value);
                        return;
                    }
                }
            }
        }
        else
            c->vValue = value; //change the current value if association exists
    }
    
    // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const
    {
        node* temp = findAppear(key); //find the pointer to the node with key
        if (temp == nullptr)
            return nullptr;
        return &(temp->vValue); //return the ValueType if the node exists
    }
    
    // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
    }
    
    // C++11 syntax for preventing copying and assignment
    MyMap(const MyMap&) = delete;
    MyMap& operator=(const MyMap&) = delete;
    
private:
    struct node
    {
        KeyType kValue;
        ValueType vValue;
        node(KeyType k, ValueType v)
        {
            kValue =k;
            vValue = v;
            left = nullptr;
            right = nullptr;
        }
        node* left;
        node* right;
    };
    node* root;
    int m_size;
    void clear(node* p)
    {
        if (p != nullptr)
        {
            //use recursion, and delete from the end of the tree
            if(p->left != nullptr)
                clear(p->left);
            if(p->right != nullptr)
                clear(p->right);
        }
        delete p;
    }
    node* findAppear(const KeyType& key) const
    {
        node* temp = root;
        while (temp != nullptr)
        {
            if(key == temp->kValue)
                break;
            else if (key < temp->kValue)
                temp = temp->left;
            else 
                temp = temp->right;
        }
        if (temp == nullptr)
            return nullptr;
        return temp;
    }
};
#endif /* MyMap_h */