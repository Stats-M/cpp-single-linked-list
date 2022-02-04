#pragma once

#include <utility>
#include <algorithm>
#include <initializer_list>
#include <iterator>

template <typename Type>
class SingleLinkedList
{
    // Узел списка
    struct Node
    {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next)
        {}
        Type value{};
        Node* next_node = nullptr;
    };

public:

    // Конструктор по-умолчанию
    SingleLinkedList() : head_({}), size_(0u)
    {}

    // Конструирование односвязного списка на основе initializer_list
    SingleLinkedList(std::initializer_list<Type> values)
    {
        // Реализуйте конструктор самостоятельно
        Assign(values.begin(), values.end());
    }

    // Конструирование односвязного списка на основе другого списка
    SingleLinkedList(const SingleLinkedList& other)
    {
        // Реализуйте конструктор самостоятельно
        Assign(other.begin(), other.end());
    }

    ~SingleLinkedList()
    {
        Clear();
    }

    // Как написать собственный итератор в стиле С++17. С С++20 уже используются концепты.
    // https://www.internalpointers.com/post/writing-custom-iterators-modern-cpp

    // Шаблон класса Базовый Итератор.
    // Определяет поведение итератора на элементы односвязного списка
    // ValueType - совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
    template <typename ValueType>
    class BasicIterator
    {

    private:
        // Узел, на который указывает итератор
        Node* node_ = nullptr;

        // Класс списка объявляется дружественным, чтобы из методов списка
        // был доступ к приватной области итератора
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node)
        {
            // Реализуйте конструктор самостоятельно
            node_ = node;
        }

    public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

        // Категория итератора - forward iterator
        // (итератор, который поддерживает операции инкремента и многократное разыменование)
        // Категория итератора сообщает стандартной библиотеке о наборе операций, поддерживаемых итератором.
        // Так как односвязный список поддерживает перебор элементов только в прямом направлении, 
        // его категория итератора должна быть равна std::forward_iterator_tag
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор 
        // В случае односвязного списка этот тип совпадает с параметром Type шаблона SingleLinkedList
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами. Используется в таких функциях 
        // как std::advance и std::distance. Для SingleLinkedList можно использовать тип std::ptrdiff_t.
        // Это целое число со знаком, разрядность которого совпадает с разрядностью указателя на целевой платформе
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение (тип указателя, возвращаемого операцией ->). 
        // Для неконстантного итератора SingleLinkedList это Type*, а для константного — const Type*
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение (которую возвращает операция разыменования *).
        // В зависимости от константности итератора SingleLinkedList это либо Type&, либо const Type&
        using reference = ValueType&;

        // Конструктор по-умолчанию, создается компилятором
        BasicIterator() = default;

        // Конвертирующий конструктор/конструктор копирования
        // При ValueType, совпадающем с Type, играет роль копирующего конструктора
        // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
        BasicIterator(const BasicIterator<Type>& other) noexcept
            : node_(other.node_)
        {}

        // Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
        // пользовательского конструктора копирования, явно объявим оператор = и
        // попросим компилятор сгенерировать его за нас.
        BasicIterator& operator=(const BasicIterator& rhs) = default;

        // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка, либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept
        {
            // Реализуйте оператор самостоятельно
            return node_ == rhs.node_;
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен ==
        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept
        {
            // Реализуйте оператор самостоятельно
            return node_ != rhs.node_;
        }

        // Оператор сравнения итераторов (в роли второго аргумента итератор)
        // Два итератора равны, если они ссылаются на один и тот же элемент списка, либо на end()
        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept
        {
            // Реализуйте оператор самостоятельно
            return node_ == rhs.node_;
        }

        // Оператор проверки итераторов на неравенство
        // Противоположен ==
        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept
        {
            // Реализуйте оператор самостоятельно
            return node_ != rhs.node_;
        }

        // Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
        // Возвращает ссылку на самого себя
        // Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
        BasicIterator& operator++() noexcept
        {
            // Реализуйте оператор самостоятельно
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        // Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка.
        // Возвращает прежнее значение итератора
        // Инкремент итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        // Принимает фиктивный параметр типа int, который не используется и только подсказывает 
        // компилятору, какая версия переопределена
        BasicIterator operator++(int) noexcept
        {
            // Реализуйте оператор самостоятельно
            // 
            // Сохраняем прежнее значение объекта для последующего возврата
            auto old_value(*this);
            ++(*this); // используем логику префиксной формы инкремента
            return old_value;
        }

        // Операция разыменования. Возвращает ссылку на текущий элемент
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] reference operator*() const noexcept
        {
            // Реализуйте оператор самостоятельно
            assert(node_ != nullptr);
            return node_->value;
        }

        // Операция доступа к члену класса. Возвращает указатель на текущий элемент списка.
        // Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
        // приводит к неопределённому поведению
        [[nodiscard]] pointer operator->() const noexcept
        {
            // Реализуйте оператор самостоятельно
            assert(node_ != nullptr);
            return &(node_->value);
        }
    };

    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    // Возвращает итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    [[nodiscard]] Iterator begin() noexcept
    {
        // Реализуйте самостоятельно
        return Iterator(head_.next_node);
    }

    // Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator end() noexcept
    {
        // Реализуйте самостоятельно
        return Iterator(nullptr);
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен end()
    // Результат вызова эквивалентен вызову метода cbegin()
    [[nodiscard]] ConstIterator begin() const noexcept
    {
        // Реализуйте самостоятельно
        return cbegin();
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    // Результат вызова эквивалентен вызову метода cend()
    [[nodiscard]] ConstIterator end() const noexcept
    {
        // Реализуйте самостоятельно
        return cend();
    }

    // Возвращает константный итератор, ссылающийся на первый элемент
    // Если список пустой, возвращённый итератор будет равен cend()
    [[nodiscard]] ConstIterator cbegin() const noexcept
    {
        // Реализуйте самостоятельно
        return ConstIterator(head_.next_node);
    }

    // Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cend() const noexcept
    {
        // Реализуйте самостоятельно
        return ConstIterator(nullptr);
    }

    // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept
    {
        //Реализуйте самостоятельно
        return Iterator(&head_);
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept
    {
        //Реализуйте самостоятельно
        return cbefore_begin();
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept
    {
        //Реализуйте самостоятельно

        // Мы внутри const метода, и значит head_ имеет тип const Node
        // с помощью const_cast мы отбрасываем этот 'const'
        return ConstIterator{ const_cast<Node*>(&head_) };
    }

    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] size_t GetSize() const noexcept
    {
        return size_;
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept
    {
        return (size_ == 0);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs)
    {
        // Реализуйте присваивание самостоятельно
        // Сравниваем адреса вместо содержимого для скорости
        if (this != &rhs)
        {
            SingleLinkedList tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    // Обменивает содержимое списков за время O(1)
    void swap(SingleLinkedList& other) noexcept
    {
        // Реализуйте обмен содержимого списков самостоятельно
        std::swap(this->size_, other.size_);
        std::swap(this->head_.next_node, other.head_.next_node);
    }

    // Вставляет элемент value в начало списка за время O(1)
    void PushFront(const Type& value)
    {
        Node* new_node_ptr = new Node(value, head_.next_node);
        head_.next_node = new_node_ptr;
        ++size_;
    }

    // Вставляет элемент value после элемента, на который указывает pos.
    // Возвращает итератор на вставленный элемент
    // Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
    Iterator InsertAfter(ConstIterator pos, const Type& value)
    {
        // Реализуйте метод самостоятельно

        // Если элемента pos.node_ не существует - это серьезная ошибка, не продолжаем
        assert(pos.node_ != nullptr_t);

        // Создаем неконстантный итератор на основе знаний класса friend SingleLinkedList об устройстве BasicIterator
        Iterator it(pos.node_);
        // Создаем новый узел, передавая ему адрес следующего за pos узла
        Node* new_node_ptr = new Node(value, (++pos).node_);
        // Изменяем указатель на следующий узел у предшествующего месту вставки узла
        it.node_->next_node = new_node_ptr;
        ++size_;
        return Iterator(new_node_ptr);
    }

    void PopFront() noexcept
    {
        //Реализуйте метод самостоятельно

        // Если список пуст, просто выходим
        if (IsEmpty())
        {
            return;
        }
        // Список НЕ пуст. head_.next_node ДОЛЖЕН указывать на элемент
        assert(head_.next_node != nullptr);

        Node* new_next_node_ptr = (head_.next_node)->next_node;
        delete head_.next_node;
        --size_;
        head_.next_node = new_next_node_ptr;
    }

    // Удаляет элемент, следующий за pos.
    // Возвращает итератор на элемент, следующий за удалённым
    Iterator EraseAfter(ConstIterator pos) noexcept
    {
        //Реализуйте метод самостоятельно

        // Если список пуст ИЛИ pos не указывает на элемент ИЛИ нечего удалять после pos
        if ((IsEmpty()) || (pos.node_ == nullptr) || (pos.node_->next_node == nullptr))
        {
            // Возвращаем итератор end()
            return end();
        }

        // Создаем неконстантный итератор на основе знаний класса friend SingleLinkedList об устройстве BasicIterator
        Iterator it(pos.node_);
        // Запоминаем указатель на удаляемый элемент
        Node* removing_node_ptr = it.node_->next_node;
        // Перезаписываем указатель на следующий элемент указателем на следующий за удаляемым
        it.node_->next_node = removing_node_ptr->next_node;
        delete removing_node_ptr;
        --size_;
        return Iterator(++it);
    }

    // Очищает список за время O(N)
    void Clear() noexcept
    {
        Node* tmp_ptr;
        while (head_.next_node != nullptr)
        {
            tmp_ptr = head_.next_node->next_node;
            delete head_.next_node;
            head_.next_node = tmp_ptr;
        }
        size_ = 0;
    }

private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_;


    // Шаблонный метод копирования списков (за 1 прямой проход)
    template <typename InputIterator>
    void Assign(InputIterator from, InputIterator to)
    {
        SingleLinkedList<Type> tmp;

        // Указатель на поле "следующий узел" последнего элемента
        // (адрес ячейки памяти next_node последнего элемента списка)
        Node** ptr_to_next_node_field = &tmp.head_.next_node;

        // Обходим в цикле все итераторы, вставляя элементы в конец списка
        while (from != to)
        {
            // Создаем новый элемент на основе текущего итератора и nullptr (новый конец списка)
            Node* new_node = new Node(*from, nullptr);
            // Запоминаем адрес нового последнего элемента списка
            *ptr_to_next_node_field = new_node;
            // Запоминаем новый конец списка
            ptr_to_next_node_field = &new_node->next_node;

            // Увеличиваем счетчик элементов
            ++tmp.size_;
            // Сдвигаем итератор к следующему элементу
            ++from;
        }

        swap(tmp);
    }
};


template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept
{
    // Реализуйте шаблонную функцию swap самостоятельно
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    // Реализуйте сравнение самостоятельно

    // Теперь, когда список поддерживает итераторы, реализовать операцию == можно, 
    // применив алгоритм std::equal
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    // Реализуйте сравнение самостоятельно

    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    // Реализуйте сравнение самостоятельно

    return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    // Реализуйте сравнение самостоятельно

    // выражение list1 <= list2 противоположно list2 < list1
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    // Реализуйте сравнение самостоятельно

    // выражение list1 > list2 эквивалентно list2 < list1
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    // Реализуйте сравнение самостоятельно

    // выражение list1 >= list2 противоположно list2 > list1
    return !(rhs > lhs);
}

