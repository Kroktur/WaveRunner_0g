#pragma once
#include <type_traits>
#include <atomic>
#include "Hasher.h"
#include <string>
/**
 * @brief template struct declaration used to register a type for Hasher Rtti with the following macro
 * only the type that override this declaration can use Hasher Rtti
 * @tparam T Type to register
 */
template<typename T>
struct TypeName;

/**
 * @brief the macro to override the TypeName struct declaration
 * @tparam T Type to register
 */
#define REGISTER_RTTI(T)                   \
template<>                                 \
struct TypeName<T>                         \
{                                          \
	static constexpr std::string Name()    \
	{                                      \
		return #T;                         \
	}                                      \
};

/**
 * @brief this concept verify if the implementation of TypeName struct is found at compileTime 
 * @tparam T type to verify
 */
template<typename T>
concept RTTI_Type = requires
{
	{TypeName<T>::Name()};
};

namespace KGR
{
	namespace RTTI
	{
		/**
		 * @brief the counter version of RTTI that dependant of run time execution
		 * thread safe 
		 */
		struct Counter
		{
			/**
			 * @brief main function to get the id
			 * @tparam Type the Type we want get the id from
			 * @return size_t the id
			 */
			template<typename Type>
			static size_t GetTypeId();

			/**
			 * @brief the id of the type of the value
			 * @tparam Type type deduction 
			 * @param value the value we want to verify the type id from
			 * @return size_t the id
			 */
			template<typename Type>
			static size_t GetTypeId(const Type& value);

			/**
			 * @brief verify if a value is the type of 
			 * @tparam Type the Type we want to verify 
			 * @tparam typeTest type deduction
			 * @param value the value we want to verify the type id from
			 * @return boolean
			 */
			template<typename Type, typename typeTest >
			static bool IsType(const typeTest& value);


		private:
			static std::atomic_size_t m_id;
		};

		/**
		 * @brief the hasher version of RTTI that doesn't dependant of run time execution but need macro to initialize type
		 * thread safe
		 */
		struct Hasher
		{
			/**
			 * @brief the main function to get the id of a register type
			 * @tparam Type the Type we want get the id from
			 * @return the id 
			 */
			template<RTTI_Type Type>
			static std::uint64_t GetTypeId();

			/**
			 * @brief the id of the type of the value
			 * @tparam Type type deduction
			 * @param value the value we want to verify the type id from
			 * @return the id of the type
			 */
			template<typename Type>
			static std::uint64_t GetTypeId(const Type& value);

			/**
			 * @brief verify if a value is the type of
			 * @tparam Type the Type we want to verify
			 * @tparam typeTest type deduction
			 * @param value the value we want to verify the type id from
			 * @return boolean
			 */
			template<typename Type, typename typeTest >
			static bool IsType(const typeTest& value);
		};

		/**
		 * @brief forward declaration for an instance of a type
		 * @tparam Type the type we want instance from
		 */
		template<typename Type>
		class Instance;

		/**
		 * @brief inherit from this class if you want to have a count of instantiation  
		 * @tparam Type type we want to track from
		 */
		template<typename Type>
		struct Entity
		{
			friend Type;
			/**
			 * @brief return the instance id
			 * @return size_t
			 */
			size_t GetInstanceId() const;

		private:
			/**
			 * private constructor for because crtp
			 */
			Entity();
			size_t m_instanceId;
		};

		/**
		 * @brief instance class only use by Entity class
		 * @tparam Type type we want to track from
		 */
		template<typename Type>
		class Instance
		{
		public:
			friend Entity<Type>;
			/**
			 * @brief the total count of instance of the type
			 * @return size_t total count
			 */
			static size_t InstanceCount();

		private:
			/**
			 * increment the id only call by entity constructor
			 * @return size_t the id
			 */
			static size_t GetId();
			static std::atomic_size_t m_id;
		};
		static std::atomic_size_t m_id;
	}

	inline std::atomic_size_t RTTI::Counter::m_id = 0;

	template <typename Type>
	size_t RTTI::Counter::GetTypeId()
	{
		static size_t id = m_id++;
		return id;
	}

	template <typename Type>
	size_t RTTI::Counter::GetTypeId(const Type& value)
	{
		return  Counter::GetTypeId<Type>();
	}

	template <typename Type, typename typeTest>
	bool RTTI::Counter::IsType(const typeTest& value)
	{
		return Counter::GetTypeId(value) == Counter::GetTypeId<Type>();
	}

	template <RTTI_Type Type>
	std::uint64_t RTTI::Hasher::GetTypeId()
	{
		static std::string name = TypeName<Type>::Name();
		static std::uint64_t id = Hash::FNV1aHash(name.c_str(), name.size());
		return id;
	}

	template <typename Type>
	std::uint64_t RTTI::Hasher::GetTypeId(const Type&)
	{
		return  Hasher::GetTypeId<Type>();
	}

	template <typename Type, typename typeTest>
	bool RTTI::Hasher::IsType(const typeTest& value)
	{
		return Hasher::GetTypeId(value) == Hasher::GetTypeId<Type>();
	}

	template <typename Type>
	size_t RTTI::Entity<Type>::GetInstanceId() const
	{
		return m_instanceId;
	}

	template <typename Type>
	RTTI::Entity<Type>::Entity(): m_instanceId(RTTI::Instance<Type>::GetId())
	{

	}

	template <typename Type>
	size_t RTTI::Instance<Type>::InstanceCount()
	{
		return m_id;
	}

	template <typename Type>
	size_t RTTI::Instance<Type>::GetId()
	{
		return m_id++;
	}

	template<typename Type>
	inline std::atomic_size_t RTTI::Instance<Type>::m_id = 0;

}