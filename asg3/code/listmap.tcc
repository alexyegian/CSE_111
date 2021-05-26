//Alex Soliz ID: amsoliz
//Alexander Yegian ID: ayegian
// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

template <typename key_t, typename mapped_t, class less_t>
listmap<key_t, mapped_t, less_t>::~listmap() {
    DEBUGF('l', reinterpret_cast<const void*> (this));
    iterator itor = begin();
    while (itor != end()) {
        itor = erase(itor);
    }
}

template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t, mapped_t, less_t>::iterator
listmap<key_t, mapped_t, less_t>::insert(const value_type& pair) {
    DEBUGF('l', &pair << "->" << pair);
    auto x = begin();
    for (; x != end(); ++x) {
        if (less(pair.first, x->first)) {
            node* m = x.get_where();
            node* new_node = new node(m, m->prev, pair);
            m->prev->next = new_node;
            m->prev = new_node;
            return --x;
        }
        else if (!less(x->first, pair.first)) {
            x->second = pair.second;
            return x;
        }
    }
    node* m = x.get_where();
    node* new_node = new node(m, m->prev, pair);
    m->prev->next = new_node;
    m->prev = new_node;
    return --x;
}

template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t, mapped_t, less_t>::iterator
listmap<key_t, mapped_t, less_t>::find(const key_type& key) {
    auto x = begin();
    for (; x != end(); ++x) {
        if (less(key, x->first)) continue;
        else if (!less(x->first, key)) {
            return x;
        }
    }
    return end();
}

template <typename key_t, typename mapped_t, class less_t>
void listmap<key_t, mapped_t, 
    less_t>::printValue(const mapped_type& value) {
    auto x = begin();
    for (; x != end(); ++x) {
        if (less(value, x->second)) continue;
        else if (!less(x->second, value)) {
            cout << x->first << " = " << x->second << endl;
        }
    }
}

template <typename key_t, typename mapped_t, class less_t>
void listmap<key_t, mapped_t, less_t>::printall() {
    auto beg = begin();
    for (; beg != end(); ++beg) {
        cout << beg->first << " = " << beg->second << endl;
    }
}

template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t, mapped_t, less_t>::iterator
listmap<key_t, mapped_t, less_t>::erase(iterator position) {
    DEBUGF('l', &*position);
    auto x = begin();
    for (; x != end(); ++x) {
        if (x == position) {
            node* del = x.get_where();
            ++x;
            del->prev->next = del->next;
            del->next->prev = del->prev;
            delete del;
            return x;
        }
    }
    return x;
}


