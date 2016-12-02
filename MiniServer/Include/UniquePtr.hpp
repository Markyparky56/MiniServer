#pragma once
#include "GenericMemory.hpp"
// Rewritten UniquePtr Class Based on the implementation in Unreal Engine 4
// Was not copy-pasted

// Simple-ownership smart pointer similar to std::unique_ptr
// Non-copyable, ownership can only be transferred via a move operation

typedef decltype(nullptr) nullptr_t;

template< typename T >
class UniquePtr
{
    template< typename OtherT >
    friend class UniquePtr;

public:
    // Default Construtor - Initialises to null
    inline UniquePtr()
        : Ptr(nullptr)
    {
    }

    // Pointer constructor - Takes ownserhsip fo the pointed-to object
    explicit inline UniquePtr(T* InPtr)
        : Ptr(InPtr)
    {
    }

    // Nullptr constructor - Initalises  to null
    inline UniquePtr(nullptr_t null)
        : Ptr(null)
    {
    }

    // Move constructor
    inline UniquePtr(UniquePtr&& Other)
        : Ptr(Other.Ptr)
    {
        Other.Ptr = nullptr;
    }

    // Constructor from rvalues of other (usually derived) types
    template< typename OtherT >
    inline UniquePtr(UniquePtr<OtherT>&& Other)
        : Ptr(Other.Ptr)
    {
        Other.Ptr = nullptr;
    }

    // Move Assignment operator
    inline UniquePtr& operator=(UniquePtr&& Other)
    {
        if (this != &Other)
        {
            // We delete last, because we don't wat odd side effects
            // if the destructor of T relies on the stae of this or Other
            T* OldPtr = Ptr;
            Ptr = Other.Ptr;
            Other.Ptr = nullptr;
            delete OldPtr;
        }

        return *this;
    }

    // Asigned operator for rvalues of other (usually derived) types
    template< typename OtherT >
    inline UniquePtr& operator=(UniquePtr<OtherT>&& Other)
    {
        // We delete last, see above for why
        T* OldPtr = Ptr;
        Ptr = Other.Ptr;
        Other.Ptr = nullptr;
        delete OldPtr;

        return *this;
    }

    // Nullptr assignment operator
    inline UniquePtr& operator=(nullptr_t null)
    {
        // We delete last, see above for why
        T* OldPtr = Ptr;
        Ptr = nullptr;
        delete OldPtr;

        return *this;
    }

    // Destructor
    inline ~UniquePtr()
    {
        delete Ptr;
    }

    // Test if the UniquePtr currently owns an object
    bool IsValid() const
    {
        return Ptr != nullptr;
    }

    // bool operator
    explicit inline operator bool() const
    {
        return IsValid();
    }

    // Logical not operator
    inline bool operator!() const
    {
        return !IsValid();
    }

    // Indirection operator
    inline T* operator->() const
    {
        return Ptr;
    }

    // Dereference operator
    inline T& operator*() const
    {
        return *Ptr;
    }

    // Return a pointer to the owned object without relinquishing ownership
    inline T* Get() const
    {
        return Ptr;
    }

    // Relinquish control of the owned object to the called and set self to null
    inline T* Release()
    {
        T* Result = Ptr;
        Ptr = nullptr;
        return Result;
    }

    // Give the UniquePtr a new object (call be nullptr) and destroy the previous
    // object
    inline void Reset(T* InPtr = nullptr)
    {
        // Delete last, see above for why
        T* OldPtr = Ptr;
        Ptr = InPtr;
        delete OldPtr;
    }

private:
    // Non-copyable
    UniquePtr(const UniquePtr&);
    UniquePtr& operator=(const UniquePtr&);

    T* Ptr;
};

// Equality comparison operator
// Lhs - first UniquePtr to compare
// Rhs - second UniquePtr to compare
template< typename LhsT, typename RhsT >
inline bool operator==(const UniquePtr<LhsT>& Lhs, const UniquePtr<RhsT>& Rhs)
{
    return Lhs.Get() == Rhs.Get();
}

template< typename T>
inline bool operator==(const UniquePtr<T>& Lhs, const UniquePtr<T>& Rhs)
{
    return Lhs.Get() == Rhs.Get();
}

// Inequality comparison operator
// Lhs - first UniquePtr to compare
// Rhs - second UniquePtr to compare
template< typename LhsT, typename RhsT >
inline bool operator!=(const UniquePtr<LhsT>& Lhs, const UniquePtr<RhsT>& Rhs)
{
    return Lhs.Get() != Rhs.Get();
}
template< typename T >
inline bool operator!=(const UniquePtr<T>& Lhs, const UniquePtr<T>& Rhs)
{
    return Lhs.Get() != Rhs.Get();
}

// Equality comparison against nullptr
template< typename T >
inline bool operator==(const UniquePtr<T>& Lhs, nullptr_t null)
{
    return !Lhs.IsValid();
}
template< typename T >
inline bool operator==(nullptr_t null, const UniquePtr<T>& Rhs)
{
    return !Rhs.IsValid();
}

// Inequality comparison against nullptr
template< typename T >
inline bool operator!=(const UniquePtr<T>& Lhs, nullptr_t null)
{
    return Lhs.IsValid();
}
template< typename T >
inline bool operator!=(nullptr_t null, const UniquePtr<T>& Rhs)
{
    return Rhs.IsValid();
}

// Probably unnecessary stuff beyond this point
// Trait which allows UniquePtr to be default constructed by memsetting to zero
//template< typename T >
//struct IsZeroConstructType<UniquePtr<T>>
//{
//    enum { Value = true };
//};
//
//// Trait which allows UniquePtr to be memcpy'able from pointers
//template< typename T >
//struct IsBitwiseConstructible<UniquePtr<T>, T*>
//{
//    enum { Value = true };
//};

// Constructs a new object with the given arguments and returns it as a UniquePtr
template< typename T, typename... TArgs >
inline UniquePtr<T> MakeUnique(TArgs&&... Args)
{
    return UniquePtr<T>(new T(Forward<TArgs>(Args)...));
}

// If varidic templates aren't available these overloads will support passing up to
// 4 parameters to a constructor
//template< typename T, typename TArg0 >
//inline UniquePtr<T> MakeUnique(TArg0&& Arg0)
//{
//	return UniquePtr<T>(new T(Forward<TArg0>(Arg0)));
//}
//
//template< typename T, typename TArg0, typename TArg1 >
//inline UniquePtr<T> MakeUnique(TArg0&& Arg0, TArg1&& Arg1)
//{
//	return UniquePtr<T>(new T(Forward<TArg0>(Arg0), Forward<TArg1>(Arg1));
//}
//
//template< typename T, typename TArg0, typename TArg1, typename TArg2 >
//inline UniquePtr<T> MakeUnique(TArg0&& Arg0)
//{
//	return UniquePtr<T>(new T(Forward<TArg0>(Arg0), Forward<TArg1>(Arg1), Forward<TArg2>(Arg2));
//}
//
//template< typename T, typename TArg0, typename TArg1, typename TArg2, typename TArg3 >
//inline UniquePtr<T> MakeUnique(TArg0&& Arg0)
//{
//	return UniquePtr<T>(new T(Forward<TArg0>(Arg0), Forward<TArg1>(Arg1), Forward<TArg2>(Arg2), Forward<TArg3>(Arg3));
//}
