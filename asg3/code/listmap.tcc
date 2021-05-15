// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
}

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t, mapped_t, less_t>::iterator
listmap<key_t, mapped_t, less_t>::insert(const value_type& pair) {
    DEBUGF('l', &pair << "->" << pair);
    //DECLARE A NEW ITERATOR FOR THE INSERT
    //LOOP THROUGH ITERATORS FROM BEGIN TO END
    //loop through iterators, with x being most recent, and y being new
    //if(less(y, x)): insert y here
    //else if(less(x, y)): replace value in x with value in y
    auto x = begin();
    for (; x != end(); ++x) {
        if (less(pair.first, x->first)) {
            //new one       
            node* m = x.get_where();
            node* new_node = new node(m, m->prev, pair);
            //x->prev->next = *new_node;
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
void listmap<key_t, mapped_t, less_t>::printValue(const mapped_type& value) {
    auto x = begin();
    for (; x != end(); ++x) {
        if (less(value, x->second)) continue;
        else if (!less(x->second, value)) {
            cout << x->first << ": " << x->second << endl;
        }
    }
}

template <typename key_t, typename mapped_t, class less_t>
void listmap<key_t, mapped_t, less_t>::printall() {
    auto beg = begin();
    for (; beg != end(); ++beg) {
        cout << *beg << endl;
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
            //printf("DELETE THIS EL:");
            cout << del->value << endl;
            //delete here
            del->prev->next = del->next;
            //free(del->value);
            free(del);
            ++x;
            return x;
        }
    }
    return x;
}


