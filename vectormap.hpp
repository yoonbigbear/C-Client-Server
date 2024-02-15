// 배열 사이즈 재할당으로 인해 소멸자가 호출되는 문제를 해결할 용도로 제작중.
template <typename KEY, typename VALUE>
class Dictionary4
{
  struct /*__declspec(align(std::hardware_destructive_interference_size))*/ DataPair
  {
    DataPair() = delete;
    DataPair(DataPair&) = delete;
    DataPair(DataPair&&) = delete;
    DataPair& operator=(DataPair&& rhs) = delete;
    DataPair& operator=(DataPair& rhs) = delete;
    ~DataPair() noexcept {};
    KEY first;
    VALUE second;
  };
public:
  inline DataPair* operator[](uint32_t idx) const { return values + idx; }

  inline uint32_t size() const { return cursor; }
  inline DataPair* begin() const { return values; }
  inline DataPair* end() const { return values + cursor; }
  inline DataPair* back() const { return values + cursor -1; }

  inline void reserve(uint32_t size) 
  { 
    if (values)
      ::free(values);
    values = reinterpret_cast<DataPair*>(::malloc(size * sizeof(DataPair)));
    capacity_ = size;
  }
  template<typename _KEY>
  inline bool contains(_KEY&& k)
  {
    KEY key(std::forward<_KEY>(k));
    return keypairs.contains(key);
  }
  Dictionary4()
  {
    values = reinterpret_cast<DataPair*>(::malloc(sizeof(DataPair)));
  }
  ~Dictionary4()
  {
    for (int i = 0; i < cursor; ++i)
    {
      values[i].~DataPair();
    }
    ::free(values);
    values = nullptr;
  }

  template<typename _KEY>
  DataPair* find(_KEY&& k)
  {
    auto it = keypairs.find(std::forward<_KEY>(k));
    if (it != keypairs.end())
      return values + it->second;
    return end();
  }

  // std::vector::emplace_back과 동일한 방식으로 이미 할당된 메모리에
  // VALUE의 생성자를 호출해서 넣는다.
  template<typename _KEY, class... _VALUE>
  decltype(auto) emplace(_KEY&& k, _VALUE&&... param)
  {

    auto key = std::forward<_KEY>(k);
    void* ptr = &values[cursor].second;
    size_t remain = 0;
    if (keypairs.contains(key))
    {//기존의 key에 덮어 씌운다.
      values[keypairs[key]].first = key;
      remain = capacity_ - cursor;
      ::new(&values[cursor].second) VALUE(std::forward<_VALUE>(param)...);
    }
    else
    {
      if (capacity_ == cursor)
      {//capacity가 부족하면 2배 추가
        capacity_ <<= 1;

        DataPair* old = values;
        values = reinterpret_cast<DataPair*>(::malloc(capacity_ * sizeof(DataPair)));
        ::memmove(values, old, cursor * sizeof(DataPair));//데이터 이동

        if (old != values)
        {//기존 메모리 할당 해제
          ::free(old);
          old = nullptr;
        }
      }

      //데이터 삽입
      values[cursor].first = key;
      keypairs[key] = cursor;
      remain = capacity_ - cursor;
      ::new(&values[cursor].second) VALUE(std::forward<_VALUE>(param)...);
      cursor++;
    }

    
    //if (std::align(alignof(DataPair), sizeof(DataPair), ptr, remain))
    //{

    //}
    //::new(ptr) VALUE(std::forward<_VALUE>(param)...);
  }

  template<typename _KEY>
  void erase(_KEY&& k)
  {
    KEY key(std::forward<_KEY>(k));
    if (keypairs.contains(key))
    {
      uint32_t idx = keypairs[key]; // 지울 key의 DataPair 인덱스.

      DataPair* back = (values + cursor - 1); // 현재 back
      uint32_t back_key = back->first;        // back에서 담고있던 key hash

      values[idx].~DataPair();                // valuepair 데이터 소멸자 호출

      values[idx].first = back->first;        // 소멸시킨 위치 back 데이터로 교체
      values[idx].second = back->second;

      keypairs[back_key] = idx;               // key hash 데이터도 변경
      keypairs.erase(key);

      cursor--;                               // 커서 이동.
    }
  }

private:
  std::unordered_map<KEY, uint32_t> keypairs;
  DataPair* values;
  uint32_t cursor = 0;
  uint32_t capacity_ = 1;
};

static int k;
static int c;
struct A
{
  A() {
    c++;
  }
  A(int i) {
    a = 999;
  }
  A(const A& a) = default;
  A& operator=(A&) = default;
  A(A&& a) = default;

  ~A(){
    k++;
  }

  int a = 0;
  char h[531] = "Hi\0";
};
