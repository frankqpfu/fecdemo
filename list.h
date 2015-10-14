#ifndef galaxy_list_h
#define galaxy_list_h

template<typename T>
typedef struct
{
	T elem;
	struct record_elem<T> * prev;
	struct record_elem<T> * next;
}record_elem;

typedef struct record_elem<T>* iterator;
typedef const struct record_elem<T>* const_iterator;

/** return value?
 */
typedef struct record_elem<T> * (TraverseFunc*)(struct record_elem<T> * it);

template<typename T>
class galaxy_list
{

public:
    galaxy_list() 
    {
        Init();
    }

	struct record_elem<T> * _AllocElem()
	{
		struct record_elem<T> * elem = new record_elem < T > ;
		return elem;
	}

	void _FreeElem(struct record_elem<T> * &elem)
	{
		delete elem;
		elem = NULL;
	}

    void Init()
    {
        head = _AllocElem();
		tail = _AllocElem();
        head->next = tail;
        head->prev = NULL;
        tail->next = NULL;
        tail->preve = head;
    }

    void InsertBack(T elem)
    {
		record_elem * new_elem = _AllocElem();
        new_elem->elem = elem;
        tail->prev->next = new_elem;
        new_elem->next = tail;
        new_elem->prev = tail->prev;
        tail->prev = new_elem;
    }

	void RemoveElem(struct record_elem<T>* &it)
	{
		it->prev->next = it->next;
		it->next->prev = it->prev;

		_FreeElem(it);
	}

	void Traverse(TraverseFunc funCallback)
	{
		struct record_elem<T> * it = head;
		while (it != tail)
		{
			struct record_elem<T> * itNext = it->next;
			funcCallback(it);
			it = itNext;
		}
	}

	void Clear()
	{
		struct record_elem<T> * it = head;
		while (it != tail)
		{
			struct record_elem<T> * itNext = it->next;
			RemoveElem(it);
			it = itNext;
		}
	}
private:
    struct record_elem<T> * head;
    struct record_elem<T> * tail;

};
#endif
