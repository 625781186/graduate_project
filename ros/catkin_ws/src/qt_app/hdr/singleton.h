#ifndef SINGLETON_H
#define SINGLETON_H

template<class T>
class Singleton {
private:
    Singleton();
//    Singleton(const T &);
public:
    static T* GetInstance() {
        static T* instance = new T();  //C++11��׼��local static�����ʼ���ڶ��߳������°�ȫ
        return instance;
    }

private:
    //class members
};

#endif // SINGLETON_H
