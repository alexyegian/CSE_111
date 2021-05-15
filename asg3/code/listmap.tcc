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
            return iterator();
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
    return iterator();
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);
   return iterator();
}


