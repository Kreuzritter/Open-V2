#pragma once

constexpr int32_t vector_size = 8;
using int_vector_internal = __m256i;
using fp_vector_internal = __m256;

constexpr int32_t full_mask = 0x00FF;
constexpr int32_t empty_mask = 0;

struct int_vector;
struct fp_vector {
	fp_vector_internal value;

	__forceinline fp_vector() : value(_mm256_setzero_ps()) {}
	__forceinline fp_vector(bool b) : value(b ? _mm256_castsi256_ps(_mm256_set1_epi32(-1)) : _mm256_setzero_ps()) {}
	__forceinline constexpr fp_vector(fp_vector_internal v) : value(v) {}
	__forceinline fp_vector(float v) : value(_mm256_set1_ps(v)) {}
	__forceinline constexpr operator fp_vector_internal() {
		return value;
	}
	__forceinline float reduce() {
		// source: Peter Cordes
		__m128 vlow = _mm256_castps256_ps128(value);
		__m128 vhigh = _mm256_extractf128_ps(value, 1); // high 128
		__m128 v = _mm_add_ps(vlow, vhigh);  // add the low 128

		__m128 shuf = _mm_movehdup_ps(v); // broadcast elements 3,1 to 2,0
		__m128 sums = _mm_add_ps(v, shuf);
		shuf = _mm_movehl_ps(shuf, sums); // high half -> low half
		sums = _mm_add_ss(sums, shuf);
		return _mm_cvtss_f32(sums);
	}
	__forceinline float operator[](uint32_t i) {
		return value.m256_f32[i];
	}
	template<typename F>
	__forceinline auto visit(F&& f)->std::conditional_t<std::is_same_v<decltype(f(0.0f)), float> || std::is_same_v<decltype(f(0.0f)), bool>, fp_vector, int_vector>;
};

struct int_vector {
	int_vector_internal value;

	__forceinline int_vector() : value(_mm256_setzero_si256()) {}
	__forceinline constexpr int_vector(int_vector_internal v) : value(v) {}
	__forceinline int_vector(int32_t v) : value(_mm256_set1_epi32(v)) {}

	__forceinline constexpr operator int_vector_internal() {
		return value;
	}
	__forceinline constexpr operator fp_vector() {
		return _mm256_cvtepi32_ps(value);
	}

	template<typename F>
	__forceinline auto visit(F&& f) -> std::conditional_t<std::is_same_v<decltype(f(0)), float> || std::is_same_v<decltype(f(0)), bool>, fp_vector, int_vector> {
		if constexpr(std::is_same_v<decltype(f(0)), float>) {
			return _mm256_setr_ps(
				f(value.m256i_i32[0]),
				f(value.m256i_i32[1]),
				f(value.m256i_i32[2]),
				f(value.m256i_i32[3]),
				f(value.m256i_i32[4]),
				f(value.m256i_i32[5]),
				f(value.m256i_i32[6]),
				f(value.m256i_i32[7]));
		} else if constexpr(std::is_same_v<decltype(f(0)), bool>) {
			return _mm256_castsi256_ps(_mm256_setr_epi32(
				-int32_t(f(value.m256i_i32[0])),
				-int32_t(f(value.m256i_i32[1])),
				-int32_t(f(value.m256i_i32[2])),
				-int32_t(f(value.m256i_i32[3])),
				-int32_t(f(value.m256i_i32[4])),
				-int32_t(f(value.m256i_i32[5])),
				-int32_t(f(value.m256i_i32[6])),
				-int32_t(f(value.m256i_i32[7]))));
		} else {
			return _mm256_setr_epi32(
				f(value.m256i_i32[0]),
				f(value.m256i_i32[1]),
				f(value.m256i_i32[2]),
				f(value.m256i_i32[3]),
				f(value.m256i_i32[4]),
				f(value.m256i_i32[5]),
				f(value.m256i_i32[6]),
				f(value.m256i_i32[7]));
		}
	}
	__forceinline int32_t operator[](uint32_t i) {
		return value.m256i_i32[i];
	}
};

template<typename F>
auto fp_vector::visit(F && f) -> std::conditional_t<std::is_same_v<decltype(f(0.0f)), float> || std::is_same_v<decltype(f(0.0f)), bool>, fp_vector, int_vector> {
	if constexpr(std::is_same_v<decltype(f(0.0f)), float>) {
		return _mm256_setr_ps(
			f(value.m256_f32[0]),
			f(value.m256_f32[1]),
			f(value.m256_f32[2]),
			f(value.m256_f32[3]),
			f(value.m256_f32[4]),
			f(value.m256_f32[5]),
			f(value.m256_f32[6]),
			f(value.m256_f32[7]));
	} else if constexpr(std::is_same_v<decltype(f(0.0f)), bool>) {
		return _mm256_castsi256_ps(_mm256_setr_epi32(
			-int32_t(f(value.m256_f32[0])),
			-int32_t(f(value.m256_f32[1])),
			-int32_t(f(value.m256_f32[2])),
			-int32_t(f(value.m256_f32[3])),
			-int32_t(f(value.m256_f32[4])),
			-int32_t(f(value.m256_f32[5])),
			-int32_t(f(value.m256_f32[6])),
			-int32_t(f(value.m256_f32[7]))));
	} else {
		return _mm256_setr_epi32(
			f(value.m256_f32[0]),
			f(value.m256_f32[1]),
			f(value.m256_f32[2]),
			f(value.m256_f32[3]),
			f(value.m256_f32[4]),
			f(value.m256_f32[5]),
			f(value.m256_f32[6]),
			f(value.m256_f32[7]));
	}
}

template<typename F>
__forceinline auto generate(uint32_t offset, F&& f) -> std::conditional_t<std::is_same_v<decltype(f(0ui32)), float> || std::is_same_v<decltype(f(0ui32)), bool>, fp_vector, int_vector> {
	if constexpr(std::is_same_v<decltype(f(0ui32)), float>) {
		return _mm256_setr_ps(
			f(offset + 0ui32),
			f(offset + 1ui32),
			f(offset + 2ui32),
			f(offset + 3ui32),
			f(offset + 4ui32),
			f(offset + 5ui32),
			f(offset + 6ui32),
			f(offset + 7ui32));
	} else if constexpr(std::is_same_v<decltype(f(0ui32)), bool>) {
		return _mm256_castsi256_ps(_mm256_setr_epi32(
			-int32_t(f(offset + 0ui32)),
			-int32_t(f(offset + 0ui32)),
			-int32_t(f(offset + 0ui32)),
			-int32_t(f(offset + 0ui32)),
			-int32_t(f(offset + 0ui32)),
			-int32_t(f(offset + 0ui32)),
			-int32_t(f(offset + 0ui32)),
			-int32_t(f(offset + 0ui32))));
	} else {
		return _mm256_setr_epi32(
			f(offset + 0ui32),
			f(offset + 1ui32),
			f(offset + 2ui32),
			f(offset + 3ui32),
			f(offset + 4ui32),
			f(offset + 5ui32),
			f(offset + 6ui32),
			f(offset + 7ui32));
	}
}

template<typename F>
__forceinline auto generate(F&& f) -> std::conditional_t<std::is_same_v<decltype(f(0ui32)), float> || std::is_same_v<decltype(f(0ui32)), bool>, fp_vector, int_vector> {
	if constexpr(std::is_same_v<decltype(f(0ui32)), float>) {
		return _mm256_setr_ps(
			f(0ui32),
			f(1ui32),
			f(2ui32),
			f(3ui32),
			f(4ui32),
			f(5ui32),
			f(6ui32),
			f(7ui32));
	} else if constexpr(std::is_same_v<decltype(f(0ui32)), bool>) {
		return _mm256_castsi256_ps(_mm256_setr_epi32(
			-int32_t(f(0ui32)),
			-int32_t(f(0ui32)),
			-int32_t(f(0ui32)),
			-int32_t(f(0ui32)),
			-int32_t(f(0ui32)),
			-int32_t(f(0ui32)),
			-int32_t(f(0ui32)),
			-int32_t(f(0ui32))));
	} else {
		return _mm256_setr_epi32(
			f(0ui32),
			f(1ui32),
			f(2ui32),
			f(3ui32),
			f(4ui32),
			f(5ui32),
			f(6ui32),
			f(7ui32));
	}
}

__forceinline fp_vector operator+(fp_vector a, fp_vector b) {
	return _mm256_add_ps(a, b);
}
__forceinline fp_vector operator-(fp_vector a, fp_vector b) {
	return _mm256_sub_ps(a, b);
}
__forceinline fp_vector operator*(fp_vector a, fp_vector b) {
	return _mm256_mul_ps(a, b);
}
__forceinline fp_vector operator/(fp_vector a, fp_vector b) {
	return _mm256_div_ps(a, b);
}

__forceinline fp_vector operator&(fp_vector a, fp_vector b) {
	return _mm256_and_ps(a, b);
}
__forceinline fp_vector operator|(fp_vector a, fp_vector b) {
	return _mm256_or_ps(a, b);
}
__forceinline fp_vector operator^(fp_vector a, fp_vector b) {
	return _mm256_xor_ps(a, b);
}
__forceinline fp_vector operator~(fp_vector a) {
	return _mm256_xor_ps(a, _mm256_cmp_ps(_mm256_setzero_ps(), _mm256_setzero_ps(), _CMP_EQ_OQ));
}
__forceinline fp_vector and_not(fp_vector a, fp_vector b) {
	return _mm256_andnot_ps(b, a);
}

__forceinline fp_vector inverse(fp_vector a) {
	return _mm256_rcp_ps(a);
}
__forceinline fp_vector sqrt(fp_vector a) {
	return _mm256_sqrt_ps(a);
}
__forceinline fp_vector inverse_sqrt(fp_vector a) {
	return _mm256_rsqrt_ps(a);
}

__forceinline fp_vector multiply_and_add(fp_vector a, fp_vector b, fp_vector c) {
	return _mm256_fmadd_ps(a, b, c);
}
__forceinline fp_vector multiply_and_subtract(fp_vector a, fp_vector b, fp_vector c) {
	return _mm256_fmsub_ps(a, b, c);
}
__forceinline fp_vector negate_multiply_and_add(fp_vector a, fp_vector b, fp_vector c) {
	return _mm256_fnmadd_ps(a, b, c);
}
__forceinline fp_vector negate_multiply_and_subtract(fp_vector a, fp_vector b, fp_vector c) {
	return _mm256_fnmsub_ps(a, b, c);
}

__forceinline fp_vector min(fp_vector a, fp_vector b) {
	return _mm256_min_ps(a, b);
}
__forceinline fp_vector max(fp_vector a, fp_vector b) {
	return _mm256_max_ps(a, b);
}
__forceinline fp_vector floor(fp_vector a) {
	return _mm256_floor_ps(a);
}
__forceinline fp_vector ceil(fp_vector a) {
	return _mm256_ceil_ps(a);
}

__forceinline fp_vector operator<(fp_vector a, fp_vector b) {
	return _mm256_cmp_ps(a, b, _CMP_LT_OQ);
}
__forceinline fp_vector operator>(fp_vector a, fp_vector b) {
	return _mm256_cmp_ps(a, b, _CMP_GT_OQ);
}
__forceinline fp_vector operator<=(fp_vector a, fp_vector b) {
	return _mm256_cmp_ps(a, b, _CMP_LE_OQ);
}
__forceinline fp_vector operator>=(fp_vector a, fp_vector b) {
	return _mm256_cmp_ps(a, b, _CMP_GE_OQ);
}
__forceinline fp_vector operator==(fp_vector a, fp_vector b) {
	return _mm256_cmp_ps(a, b, _CMP_EQ_OQ);
}
__forceinline fp_vector operator!=(fp_vector a, fp_vector b) {
	return _mm256_cmp_ps(a, b, _CMP_NEQ_OQ);
}

__forceinline fp_vector operator<(int_vector a, int_vector b) {
	return _mm256_castsi256_ps(_mm256_cmpgt_epi32(b, a));
}
__forceinline fp_vector operator>(int_vector a, int_vector b) {
	return _mm256_castsi256_ps(_mm256_cmpgt_epi32(a, b));
}
__forceinline fp_vector operator<=(int_vector a, int_vector b) {
	return _mm256_castsi256_ps(_mm256_or_si256(_mm256_cmpgt_epi32(b, a), _mm256_cmpeq_epi32(a, b)));
}
__forceinline fp_vector operator>=(int_vector a, int_vector b) {
	return _mm256_castsi256_ps(_mm256_or_si256(_mm256_cmpgt_epi32(a, b), _mm256_cmpeq_epi32(a, b)));
}
__forceinline fp_vector operator==(int_vector a, int_vector b) {
	return _mm256_castsi256_ps(_mm256_cmpeq_epi32(a, b));
}
__forceinline fp_vector operator!=(int_vector a, int_vector b) {
	return _mm256_castsi256_ps(_mm256_or_si256(_mm256_cmpgt_epi32(a, b), _mm256_cmpgt_epi32(b, a)));
}


__forceinline fp_vector select(int32_t mask, fp_vector a, fp_vector b) {
	auto repeated_mask = _mm256_set1_epi32(mask);
	const auto mask_filter = _mm256_setr_epi32(
		0x00000001, 0x00000002, 0x00000004, 0x00000008,
		0x00000010, 0x00000020, 0x00000040, 0x00000080);
	auto fp_mask = int_vector(_mm256_and_si256(repeated_mask, mask_filter)) != int_vector();
	return _mm256_blendv_ps(b, a, fp_mask);
}
__forceinline fp_vector widen_mask(int32_t mask) {
	auto repeated_mask = _mm256_set1_epi32(mask);
	const auto mask_filter = _mm256_setr_epi32(
		0x00000001, 0x00000002, 0x00000004, 0x00000008,
		0x00000010, 0x00000020, 0x00000040, 0x00000080);
	return int_vector(_mm256_and_si256(repeated_mask, mask_filter)) != int_vector();
}
__forceinline fp_vector select(fp_vector mask, fp_vector a, fp_vector b) {
	return _mm256_blendv_ps(b, a, mask);
}
__forceinline fp_vector is_non_zero(int_vector i) {
	return i != int_vector();
}

__forceinline int32_t compress_mask(fp_vector mask) {
	return _mm256_movemask_ps(mask);
}


inline constexpr uint32_t load_masks[16] = {
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000
};

template<typename F>
class alignas(__m256i) true_accumulator : public F {
private:
	__m256i value;
	uint32_t index = 0;
	int32_t accumulated_mask = 0;
public:
	bool result = false;

	true_accumulator(F&& f) : value(_mm256_setzero_si256()), F(std::move(f)) {}

	void add_value(int32_t v) {
		if(!result) {
			accumulated_mask |= (int32_t(v != 0) << index);
			value.m256i_i32[index++] = v;

			if(index == 8) {
				result = (ve::compress_mask(F::operator()(value)) & accumulated_mask) != 0;
				value = _mm256_setzero_si256();
				index = 0;
				accumulated_mask = 0;
			}
		}
	}
	void flush() {
		if(int32_t(index != 0) & ~int32_t(result)) {
			result = (ve::compress_mask(F::operator()(value)) & accumulated_mask) != 0;
			index = 0;
		}
	}
};

template<typename F>
class alignas(__m256i) false_accumulator : public F {
private:
	__m256i value;
	uint32_t index = 0;
	int32_t accumulated_mask = 0;
public:
	bool result = true;

	false_accumulator(F&& f) : value(_mm256_setzero_si256()), F(std::move(f)) {}

	void add_value(int32_t v) {
		if(result) {
			accumulated_mask |= (int32_t(v != 0) << index);
			value.m256i_i32[index++] = v;

			if(index == 8) {
				result = (ve::compress_mask(F::operator()(value)) & accumulated_mask) == accumulated_mask;
				value = _mm256_setzero_si256();
				index = 0;
				accumulated_mask = 0;
			}
		}
	}
	void flush() {
		if((index != 0) & result) {
			result = (ve::compress_mask(F::operator()(value)) & accumulated_mask) == accumulated_mask;
			index = 0;
		}
	}
};

template<typename F>
auto make_true_accumulator(F const& f) -> true_accumulator<F> {
	return true_accumulator<F>(f);
}

template<typename F>
auto make_false_accumulator(F const& f) -> false_accumulator<F> {
	return false_accumulator<F>(f);
}

template<int32_t blk_index = 0>
class full_vector_operation {
protected:
	uint32_t const offset;
public:
	full_vector_operation(uint32_t o) : offset(o) {}

	constexpr static int32_t block_index = blk_index;
	constexpr static bool full_operation = true;

	__forceinline static fp_vector zero() {
		return _mm256_setzero_ps();
	}
	__forceinline static int_vector int_zero() {
		return _mm256_setzero_si256();
	}
	__forceinline static fp_vector constant(float v) {
		return _mm256_set1_ps(v);
	}
	__forceinline static int_vector constant(int32_t v) {
		return _mm256_set1_epi32(v);
	}

	__forceinline fp_vector load(float const* source) {
		return _mm256_load_ps(source + offset);
	}
	__forceinline int32_t load(int8_t const* source) {
		return source[offset / 8ui32];
	}
	__forceinline int_vector load(int32_t const* source) {
		return _mm256_load_si256((__m256i const*)(source + offset));
	}
	__forceinline fp_vector unaligned_load(float const* source) {
		return _mm256_loadu_ps(source + offset);
	}
	__forceinline int_vector unaligned_load(int32_t const* source) {
		return _mm256_loadu_si256((__m256i const*)(source + offset));
	}

	__forceinline static fp_vector gather_load(float const* source, __m256i indices) {
		return _mm256_i32gather_ps(source, indices, 4);
	}
	__forceinline static int_vector gather_load(int32_t const* source, __m256i indices) {
		return _mm256_i32gather_epi32(source, indices, 4);
	}
	__forceinline static int32_t gather_load(int8_t const* source, __m256i indices) {
		const auto byte_indices = _mm256_srl_epi32(indices, 3);
		const auto bit_indices = _mm256_and_si256(indices, _mm256_set1_epi32(0x00000007));
		auto gathered = _mm256_i32gather_epi32(source, byte_indices, 1);
		auto shifted = _mm256_and_si256(_mm256_srlv_epi32(gathered, bit_indices), _mm256_set1_epi32(0x00000001));
		return _mm256_movemask_ps(_mm256_castsi256_ps(_mm256_sub_epi32(_mm256_setzero_si256(), shifted)));
	}

	__forceinline static fp_vector gather_masked_load(float const* source, __m256i indices, fp_vector mask, fp_vector def = _mm256_setzero_ps()) {
		return _mm256_mask_i32gather_ps(def, source, indices, mask, 4);
	}
	__forceinline static int_vector gather_masked_load(int32_t const* source, __m256i indices, int_vector mask, int_vector def = _mm256_setzero_si256()) {
		return _mm256_mask_i32gather_epi32(def, source, indices, mask, 4);
	}

	__forceinline void store(float* dest, fp_vector value) {
		_mm256_store_ps(dest + offset, value);
	}
	__forceinline void unaligned_store(float* dest, fp_vector value) {
		_mm256_storeu_ps(dest + offset, value);
	}
	
	__forceinline void scatter_store(float* dest, fp_vector to_store, __m256i indices) {
		dest[indices.m256i_i32[0]] = to_store.value.m256_f32[0];
		dest[indices.m256i_i32[1]] = to_store.value.m256_f32[1];
		dest[indices.m256i_i32[2]] = to_store.value.m256_f32[2];
		dest[indices.m256i_i32[3]] = to_store.value.m256_f32[3];
		dest[indices.m256i_i32[4]] = to_store.value.m256_f32[4];
		dest[indices.m256i_i32[5]] = to_store.value.m256_f32[5];
		dest[indices.m256i_i32[6]] = to_store.value.m256_f32[6];
		dest[indices.m256i_i32[7]] = to_store.value.m256_f32[7];
	}


	template<int32_t cache_lines>
	__forceinline void prefetch(int32_t const* source) {
		if constexpr(block_index % 2 == 0) {
			_mm_prefetch((char const*)(source + 16 * cache_lines), _MM_HINT_T0);
		}
	}
	template<int32_t cache_lines>
	__forceinline void prefetch(float const* source) {
		if constexpr(block_index % 2 == 0) {
			_mm_prefetch((char const*)(source + 16 * cache_lines), _MM_HINT_T0);
		}
	}
	template<int32_t cache_lines>
	__forceinline void nt_prefetch(int32_t const* source) {
		if constexpr(block_index % 2 == 0) {
			_mm_prefetch((char const*)(source + 16 * cache_lines), _MM_HINT_NTA);
		}
	}
	template<int32_t cache_lines>
	__forceinline void nt_prefetch(float const* source) {
		if constexpr(block_index % 2 == 0) {
			_mm_prefetch((char const*)(source + 16 * cache_lines), _MM_HINT_NTA);
		}
	}

	__forceinline fp_vector stream_load(float const* source) {
		return _mm256_castsi256_ps(_mm256_stream_load_si256((__m256i const*)(source + offset)));
	}
	__forceinline int_vector stream_load(int32_t const* source) {
		return _mm256_stream_load_si256((__m256i const*)(source + offset));
	}
	__forceinline void stream_store(float* dest, fp_vector value) {
		_mm256_stream_ps(dest + offset, value);
	}

	__forceinline int_vector partial_mask() {
		return _mm256_loadu_si256((__m256i const*)load_masks);
	}

	template<typename F>
	__forceinline fp_vector apply(F const& f, fp_vector arg) {
		return _mm256_setr_ps(
			f(arg.value.m256_f32[0], offset),
			f(arg.value.m256_f32[1], offset + 1ui32),
			f(arg.value.m256_f32[2], offset + 2ui32),
			f(arg.value.m256_f32[3], offset + 3ui32),
			f(arg.value.m256_f32[4], offset + 4ui32),
			f(arg.value.m256_f32[5], offset + 5ui32),
			f(arg.value.m256_f32[6], offset + 6ui32),
			f(arg.value.m256_f32[7], offset + 7ui32));
	}

	template<typename F>
	__forceinline fp_vector apply_for_mask(F const& f, int_vector arg) {
		return _mm256_castsi256_ps(_mm256_setr_epi32(
			f(arg.value.m256i_i32[0], offset),
			f(arg.value.m256i_i32[1], offset + 1ui32),
			f(arg.value.m256i_i32[2], offset + 2ui32),
			f(arg.value.m256i_i32[3], offset + 3ui32),
			f(arg.value.m256i_i32[4], offset + 4ui32),
			f(arg.value.m256i_i32[5], offset + 5ui32),
			f(arg.value.m256i_i32[6], offset + 6ui32),
			f(arg.value.m256i_i32[7], offset + 7ui32))
		);
	}

	template<typename F>
	__forceinline auto apply(F const& f, int_vector arg) -> std::conditional_t<std::is_same_v<decltype(f(0,0ui32)),float>, fp_vector, int_vector> {
		if constexpr(std::is_same_v<decltype(f(0, 0ui32)), float>) {
			return _mm256_setr_ps(
				f(arg.value.m256i_i32[0], offset),
				f(arg.value.m256i_i32[1], offset + 1ui32),
				f(arg.value.m256i_i32[2], offset + 2ui32),
				f(arg.value.m256i_i32[3], offset + 3ui32),
				f(arg.value.m256i_i32[4], offset + 4ui32),
				f(arg.value.m256i_i32[5], offset + 5ui32),
				f(arg.value.m256i_i32[6], offset + 6ui32),
				f(arg.value.m256i_i32[7], offset + 7ui32));
		} else {
			return _mm256_setr_epi32(
				f(arg.value.m256i_i32[0], offset),
				f(arg.value.m256i_i32[1], offset + 1ui32),
				f(arg.value.m256i_i32[2], offset + 2ui32),
				f(arg.value.m256i_i32[3], offset + 3ui32),
				f(arg.value.m256i_i32[4], offset + 4ui32),
				f(arg.value.m256i_i32[5], offset + 5ui32),
				f(arg.value.m256i_i32[6], offset + 6ui32),
				f(arg.value.m256i_i32[7], offset + 7ui32));
		}
	}

	template<typename F>
	__forceinline fp_vector generate(F const& f) {
		return _mm256_setr_ps(
			f(offset),
			f(offset + 1ui32),
			f(offset + 2ui32),
			f(offset + 3ui32),
			f(offset + 4ui32),
			f(offset + 5ui32),
			f(offset + 6ui32),
			f(offset + 7ui32));
	}
};

template<int32_t blk_index = 0>
class full_unaligned_vector_operation : public full_vector_operation<blk_index> {
public:
	full_unaligned_vector_operation(uint32_t o) : full_vector_operation<blk_index>(o) {}

	__forceinline fp_vector load(float const* source) {
		return _mm256_loadu_ps(source + full_vector_operation<blk_index>::offset);
	}
	__forceinline int_vector load(int32_t const* source) {
		return _mm256_loadu_si256((__m256i const*)(source + full_vector_operation<blk_index>::offset));
	}
	__forceinline void store(float* dest, fp_vector value) {
		_mm256_storeu_ps(dest + full_vector_operation<blk_index>::offset, value);
	}

	__forceinline fp_vector stream_load(float const* source) {
		return load(source);
	}
	__forceinline int_vector stream_load(int32_t const* source) {
		return load(source);
	}
	__forceinline void stream_store(float* dest, fp_vector value) {
		store(dest, value);
	}
};

class partial_vector_operation : public full_vector_operation<0> {
protected:
	uint32_t const count;
public:
	partial_vector_operation(uint32_t o, uint32_t c) : full_vector_operation<0>(o), count(c) {}

	constexpr static bool full_operation = false;

	__forceinline fp_vector load(float const* source) {
		int_vector_internal mask = _mm256_loadu_si256((__m256i const*)(load_masks + 8ui32 - count));
		return _mm256_maskload_ps(source + offset, mask);
	}
	__forceinline int_vector load(int32_t const* source) {
		int_vector_internal mask = _mm256_loadu_si256((__m256i const*)(load_masks + 8ui32 - count));
		return _mm256_maskload_epi32(source + offset, mask); //AVX2
	}
	__forceinline int8_t load(int8_t const* source) {
		return source[offset / 8ui32];
	}
	__forceinline void store(float* dest, fp_vector value) {
		int_vector_internal mask = _mm256_loadu_si256((__m256i const*)(load_masks + 8ui32 - count));
		_mm256_maskstore_ps(dest + offset, mask, value);
	}
	__forceinline int_vector partial_mask() {
		return _mm256_loadu_si256((__m256i const*)(load_masks + 8ui32 - count));
	}

	__forceinline fp_vector unaligned_load(float const* source) {
		return load(source);
	}
	__forceinline int_vector unaligned_load(int32_t const* source) {
		return load(source);
	}

	__forceinline fp_vector gather_load(float const* source, __m256i indices) {
		fp_vector_internal mask = _mm256_loadu_ps((float*)(load_masks + 8ui32 - count));
		return _mm256_mask_i32gather_ps(_mm256_setzero_ps(), source, indices, mask, 4);
	}
	__forceinline int_vector gather_load(int32_t const* source, __m256i indices) {
		int_vector_internal mask = _mm256_loadu_si256((__m256i const*)(load_masks + 8ui32 - count));
		return _mm256_mask_i32gather_epi32(_mm256_setzero_si256(), source, indices, mask, 4);
	}
	__forceinline fp_vector gather_masked_load(float const* source, __m256i indices, fp_vector mask, fp_vector def = _mm256_setzero_ps()) {
		fp_vector_internal imask = _mm256_loadu_ps((float const*)(load_masks + 8ui32 - count));
		return _mm256_mask_i32gather_ps(def, source, indices, mask & imask, 4);
	}
	__forceinline int_vector gather_masked_load(int32_t const* source, __m256i indices, int_vector mask, int_vector def = _mm256_setzero_si256()) {
		int_vector_internal imask = _mm256_loadu_si256((__m256i const*)(load_masks + 8ui32 - count));
		return _mm256_mask_i32gather_epi32(def, source, indices, _mm256_and_si256(mask, imask), 4);
	}

	__forceinline void unaligned_store(float* dest, fp_vector value) {
		store(dest, value);
	}

	__forceinline fp_vector stream_load(float const* source) {
		return load(source);
	}
	__forceinline int_vector stream_load(int32_t const* source) {
		return load(source);
	}
	__forceinline void stream_store(float* dest, fp_vector value) {
		store(dest, value);
	}

	template<int32_t cache_lines>
	__forceinline void prefetch(int32_t const* source) {
	}
	template<int32_t cache_lines>
	__forceinline void prefetch(float const* source) {
	}
	template<int32_t cache_lines>
	__forceinline void nt_prefetch(int32_t const* source) {
	}
	template<int32_t cache_lines>
	__forceinline void nt_prefetch(float const* source) {
	}

	__forceinline void scatter_store(float* dest, fp_vector to_store, __m256i indices) {
		switch(count) {
			case 8:
				dest[indices.m256i_i32[7]] = to_store.value.m256_f32[7];
			case 7:
				dest[indices.m256i_i32[6]] = to_store.value.m256_f32[6];
			case 6:
				dest[indices.m256i_i32[5]] = to_store.value.m256_f32[5];
			case 5:
				dest[indices.m256i_i32[4]] = to_store.value.m256_f32[4];
			case 4:
				dest[indices.m256i_i32[3]] = to_store.value.m256_f32[3];
			case 3:
				dest[indices.m256i_i32[2]] = to_store.value.m256_f32[2];
			case 2:
				dest[indices.m256i_i32[1]] = to_store.value.m256_f32[1];
			case 1:
				dest[indices.m256i_i32[0]] = to_store.value.m256_f32[0];
			default:
				break;
		}
	}

	template<typename F>
	__forceinline fp_vector apply(F const& f, fp_vector arg) {
		return _mm256_setr_ps(
			count > 0 ? f(arg.value.m256_f32[0], offset) : 0.0f,
			count > 1 ? f(arg.value.m256_f32[1], offset + 1ui32) : 0.0f,
			count > 2 ? f(arg.value.m256_f32[2], offset + 2ui32) : 0.0f,
			count > 3 ? f(arg.value.m256_f32[3], offset + 3ui32) : 0.0f,
			count > 4 ? f(arg.value.m256_f32[4], offset + 4ui32) : 0.0f,
			count > 5 ? f(arg.value.m256_f32[5], offset + 5ui32) : 0.0f,
			count > 6 ? f(arg.value.m256_f32[6], offset + 6ui32) : 0.0f,
			count > 7 ? f(arg.value.m256_f32[7], offset + 7ui32) : 0.0f);
	}

	template<typename F>
	__forceinline auto apply(F const& f, int_vector arg) -> std::conditional_t<std::is_same_v<decltype(f(0, 0ui32)), float>, fp_vector, int_vector> {
		if constexpr(std::is_same_v<decltype(f(0, 0ui32)), float>) {
			return _mm256_setr_ps(
				count > 0 ? f(arg.value.m256i_i32[0], offset) : 0,
				count > 1 ? f(arg.value.m256i_i32[1], offset + 1ui32) : 0,
				count > 2 ? f(arg.value.m256i_i32[2], offset + 2ui32) : 0,
				count > 3 ? f(arg.value.m256i_i32[3], offset + 3ui32) : 0,
				count > 4 ? f(arg.value.m256i_i32[4], offset + 4ui32) : 0,
				count > 5 ? f(arg.value.m256i_i32[5], offset + 5ui32) : 0,
				count > 6 ? f(arg.value.m256i_i32[6], offset + 6ui32) : 0,
				count > 7 ? f(arg.value.m256i_i32[7], offset + 7ui32) : 0);
		} else {
			return _mm256_setr_epi32(
				count > 0 ? f(arg.value.m256i_i32[0], offset) : 0,
				count > 1 ? f(arg.value.m256i_i32[1], offset + 1ui32) : 0,
				count > 2 ? f(arg.value.m256i_i32[2], offset + 2ui32) : 0,
				count > 3 ? f(arg.value.m256i_i32[3], offset + 3ui32) : 0,
				count > 4 ? f(arg.value.m256i_i32[4], offset + 4ui32) : 0,
				count > 5 ? f(arg.value.m256i_i32[5], offset + 5ui32) : 0,
				count > 6 ? f(arg.value.m256i_i32[6], offset + 6ui32) : 0,
				count > 7 ? f(arg.value.m256i_i32[7], offset + 7ui32) : 0);
		}
	}

	template<typename F>
	__forceinline fp_vector apply_for_mask(F const& f, int_vector arg) {
		return _mm256_castsi256_ps(_mm256_setr_epi32(
			count > 0 ? f(arg.value.m256i_i32[0], offset) : 0,
			count > 1 ? f(arg.value.m256i_i32[1], offset + 1ui32) : 0,
			count > 2 ? f(arg.value.m256i_i32[2], offset + 2ui32) : 0,
			count > 3 ? f(arg.value.m256i_i32[3], offset + 3ui32) : 0,
			count > 4 ? f(arg.value.m256i_i32[4], offset + 4ui32) : 0,
			count > 5 ? f(arg.value.m256i_i32[5], offset + 5ui32) : 0,
			count > 6 ? f(arg.value.m256i_i32[6], offset + 6ui32) : 0,
			count > 7 ? f(arg.value.m256i_i32[7], offset + 7ui32) : 0)
		);
	}

	template<typename F>
	__forceinline fp_vector generate(F const& f) {
		return _mm256_setr_ps(
			count > 0 ? f(offset) : 0.0f,
			count > 1 ? f(offset + 1ui32) : 0.0f,
			count > 2 ? f(offset + 2ui32) : 0.0f,
			count > 3 ? f(offset + 3ui32) : 0.0f,
			count > 4 ? f(offset + 4ui32) : 0.0f,
			count > 5 ? f(offset + 5ui32) : 0.0f,
			count > 6 ? f(offset + 6ui32) : 0.0f,
			count > 7 ? f(offset + 7ui32) : 0.0f);
	}
};

using partial_unaligned_vector_operation = partial_vector_operation;
