#ifndef DRAGAZO_META_PTR_H
#define DRAGAZO_META_PTR_H

#include <cstdint>
#include <cstddef>
#include <exception>
#include <stdexcept>
#include <type_traits>

// represents a pointer to T with additional meta data stored in the least-significant bits.
// specifically, n bits are available for meta data where n = log2(alignof(T)).
// attempting to assign an unaligned pointer to this object results in an exception.
template<typename T>
class meta_ptr
{
private: // -- data -- //

	std::uintptr_t raw; // the raw value containing the pointer and meta data

private: // -- helpers -- //

	// computes log2 at compile time - und if v is not a power of 2.
	static constexpr std::size_t ctime_log2(std::size_t v) { return v <= 1 ? 0 : 1 + ctime_log2(v >> 1);}

public: // -- ctor / dtor / asgn -- //

	// constructs a meta ptr with zeroed meta data
	constexpr meta_ptr(T *ptr = nullptr) { reset_clear(ptr); }

	// assigns the pointer but doesn't modify the meta data
	constexpr meta_ptr &operator=(T *ptr) { reset(ptr); return *this; }

	// gets the stored pointer (with meta data stripped)
	constexpr operator T*() const noexcept { return get(); }

	// returns true iff the stored pointer is non-null
	constexpr explicit operator bool() const noexcept { return get() != nullptr; }
	// returns true iff the stored pointer is null
	constexpr bool operator!() const noexcept { return get() == nullptr; }

	// copies the pointer and meta data
	meta_ptr(const meta_ptr&) = default;
	// copies the pointer and meta data
	meta_ptr &operator=(const meta_ptr&) = default;

public: // -- pointer data -- //

	// gets the stored pointer (with meta data stripped)
	constexpr T *get() const noexcept
	{
		return (T*)(raw & ~(alignof(T) - 1));
	}

	// sets the stored pointer (preserves meta data).
	// throws std::invalid_argument if ptr is not properly aligned for type T.
	// if an exception is thrown, no change is made.
	constexpr void reset(T *ptr = nullptr)
	{
		if ((std::uintptr_t)ptr & (alignof(T) - 1))
			throw std::invalid_argument("ptr was not aligned");

		raw = (std::uintptr_t)ptr | (raw & (alignof(T) - 1));
	}

	// sets the stored pointer and clears meta data to zero.
	// throws std::invalid_argument if ptr is not properly aligned for type T.
	// if an exception is thrown, no change is made.
	constexpr void reset_clear(T *ptr = nullptr)
	{
		if ((std::uintptr_t)ptr & (alignof(T) - 1))
			throw std::invalid_argument("ptr was not aligned");

		raw = (std::uintptr_t)ptr;
	}

	constexpr T &operator*() const { return *get(); }
	constexpr T *operator->() const noexcept { return get(); }
	constexpr T &operator[](std::ptrdiff_t index) const { get()[index]; }

	// increments the pointer value (preserves meta data)
	constexpr meta_ptr &operator++() noexcept { reset(get() + 1); return *this; }
	constexpr meta_ptr operator++(int) noexcept { meta_ptr cpy(*this); ++*this; return cpy; }

	// decrements the pointer value (preserves meta data)
	constexpr meta_ptr &operator--() noexcept { reset(get() - 1); return *this; }
	constexpr meta_ptr operator--(int) noexcept { meta_ptr cpy(*this); --*this; return cpy; }

	// adds to the pointer value (preserves meta data)
	constexpr meta_ptr &operator+=(std::ptrdiff_t off) { reset(get() + off); return *this; }
	// subtracts from the pointer value (preserves meta data)
	constexpr meta_ptr &operator-=(std::ptrdiff_t off) { reset(get() - off); return *this; }

	// adds to the pointer value (preserves meta data)
	friend constexpr meta_ptr operator+(meta_ptr p, std::ptrdiff_t off) { p += off; return p; }
	friend constexpr meta_ptr operator+(std::ptrdiff_t off, meta_ptr p) { p += off; return p; }

	// subtracts from the pointer value (preserves meta data)
	friend constexpr meta_ptr operator-(meta_ptr p, std::ptrdiff_t off) { p -= off; return p; }

public: // -- meta data -- //

	// the number of available meta data bits
	static constexpr std::size_t meta_bits = ctime_log2(alignof(T));

	// reads the meta data.
	// the low bits of the result contain the meta data (up to meta_bits).
	// the high bits are zero.
	constexpr std::size_t read() const noexcept
	{
		return raw & (alignof(T) - 1);
	}
	// writes the meta data (preserves the pointer value).
	// the low bits are written to the meta data (up to meta_bits).
	// the high bits are discarded.
	constexpr void write(std::size_t v) noexcept
	{
		raw = raw & ~(alignof(T) - 1) | v & (alignof(T) - 1);
	}

	// sets (all) the meta data bits to 1
	constexpr void set() noexcept
	{
		raw |= alignof(T) - 1;
	}
	// clears (all) the meta data bits to 0.
	constexpr void clear() noexcept
	{
		raw &= ~(alignof(T) - 1);
	}
	// flips (all) the meta data bits.
	constexpr void flip() noexcept
	{
		raw ^= alignof(T) - 1;
	}

public: // -- fine-tuned meta data -- //

	// reads the meta data bit at positon pos (0 order)
	template<std::size_t pos, std::enable_if_t<pos < meta_bits, int> = 0>
	constexpr bool read() const noexcept
	{
		return raw & ((std::uintptr_t)1 << pos);
	}
	// writes the meta data bit at position pos (0 order)
	template<std::size_t pos, std::enable_if_t<pos < meta_bits, int> = 0>
	constexpr void write(bool val) noexcept
	{
		val ? set<pos>() : clear<pos>();
	}

	// sets the meta data bit at position pos (0 order)
	template<std::size_t pos, std::enable_if_t<pos < meta_bits, int> = 0>
	constexpr void set() noexcept
	{
		raw |= (std::uintptr_t)1 << pos;
	}
	// clears the meta data bit at position pos (0 order)
	template<std::size_t pos, std::enable_if_t<pos < meta_bits, int> = 0>
	constexpr void clear() noexcept
	{
		raw &= ~((std::uintptr_t)1 << pos);
	}
	// flips the meta data bit at position pos (0 order)
	template<std::size_t pos, std::enable_if_t<pos < meta_bits, int> = 0>
	constexpr void flip() noexcept
	{
		raw ^= (std::uintptr_t)1 << pos;
	}

public: // -- comparison -- //

	// returns true iff the meta ptrs point to the same object AND have identical meta data
	constexpr friend bool operator==(meta_ptr a, meta_ptr b) { return a.raw == b.raw; }
	// returns true iff the meta ptrs point to different objects OR have different meta data
	constexpr friend bool operator!=(meta_ptr a, meta_ptr b) { return a.raw != b.raw; }

	// returns true iff the meta ptr and the raw pointer point to the same object
	constexpr friend bool operator==(meta_ptr m, T *r) { return m.get() == r; }
	constexpr friend bool operator==(T *r, meta_ptr m) { return m.get() == r; }
	// returns true iff the meta ptr and the raw pointer point to different objects
	constexpr friend bool operator!=(meta_ptr m, T *r) { return m.get() != r; }
	constexpr friend bool operator!=(T *r, meta_ptr m) { return m.get() != r; }
};

#endif
