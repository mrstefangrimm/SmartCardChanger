// Copyright (c) 2023 - 2025 Stefan Grimm. All rights reserved.
// Licensed under the LGPL. See LICENSE file in the project root for full license information.
//
// 08/28/2025 Implement queue size > 2
// 09/23/2023 Implement queue size 1 and 2

#ifndef ValueQueue_h
#define ValueQueue_h

void rtassert(bool condition);

// ItemAlloc, pop return false?
template<size_t size, class ItemType = uint8_t>
class ValueQueue {
public:
  ValueQueue() {
    static_assert(size > 2, "Queue size 0 is not supported.");
  }

  void push(ItemType item) {
    rtassert(_values[size - 1] == 0);

    for (int i = 0; i < size; i++) {
      if (_values[i] == 0) {
        _values[i] = item;
        break;
      }
    }
  }

  ItemType pop() {
    ItemType ret = _values[0];

    int i = 1;
    for (; i < size; i++) {
      if (_values[i] == 0) break;

      _values[i - 1] = _values[i];
    }

    _values[i - 1] = 0;

    return ret;
  }
  void clear() {
  }

private:
  ItemType _values[size] = { 0 };
  uint8_t _readPos = 0;
};

template<class ItemType>
class ValueQueue<0, ItemType> {
public:
  ValueQueue()
  {
    //static_assert(false, "Queue size 0 is not supported.");
  }
  void push(ItemType item) {
  }
  ItemType pop() {
    return 0;
  }
  void clear() {
  }
};

template<class ItemType>
class ValueQueue<1, ItemType> {
public:
  void push(ItemType item) {
    rtassert(_item == 0);
    _item = item;
  }
  ItemType pop() {
    ItemType ret = _item;
    _item = 0;
    return ret;
  }
  void clear() {
    _item = 0;
  }
private:
  ItemType _item = 0;
};

template<class ItemType>
class ValueQueue<2, ItemType> {
public:
  void push(ItemType item) {
    rtassert(_item1 == 0 || _item2 == 0);
    if (_item1 == 0) {
      _item1 = item;
    } else {
      _item2 = item;
    }

  }
  ItemType pop() {
    ItemType ret = _item1;
    _item1 = _item2;
    _item2 = 0;
    return ret;
  }
  void clear() {
    _item1 = 0;
    _item2 = 0;
  }
private:
  ItemType _item1 = 0;
  ItemType _item2 = 0;
};

#endif
