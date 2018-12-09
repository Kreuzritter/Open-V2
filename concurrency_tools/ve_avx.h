#pragma once

constexpr int32_t vector_size = 8;
using int_vector_internal = __m256i;
using fp_vector_internal = __m256;

struct fp_vector {
	fp_vector_internal value;

	__forceinline fp_vector() : value(_mm256_setzero_ps()) {}
	__forceinline constexpr fp_vector(fp_vector_internal v) : value(v) {}
	__forceinline fp_vector(float v) : value(_mm256_set1_ps(v)) {}
	__forceinline constexpr operator __m256() {
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
};

struct int_vector {
	int_vector_internal value;

	__forceinline constexpr int_vector(int_vector_internal v) : value(v) {}
	__forceinline int_vector(int32_t v) : value(_mm256_set1_epi32(v)) {}
	__forceinline constexpr operator __m256i() {
		return value;
	}
	__forceinline constexpr operator fp_vector() {
		return _mm256_cvtepi32_ps(value);
	}
};

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
	return _mm256_add_ps(_mm256_mul_ps(a, b), c);
}
__forceinline fp_vector multiply_and_subtract(fp_vector a, fp_vector b, fp_vector c) {
	return _mm256_sub_ps(_mm256_mul_ps(a, b), c);
}
__forceinline fp_vector negate_multiply_and_add(fp_vector a, fp_vector b, fp_vector c) {
	return _mm256_sub_ps(c, _mm256_mul_ps(a, b));
}
__forceinline fp_vector negate_multiply_and_subtract(fp_vector a, fp_vector b, fp_vector c) {
	return _mm256_sub_ps(_mm256_sub_ps(_mm256_setzero_ps(), c), _mm256_mul_ps(a, b));
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
__forceinline fp_vector select(fp_vector mask, fp_vector a, fp_vector b) {
	return _mm256_blendv_ps(b, a, mask);
}
__forceinline fp_vector select(int32_t mask, fp_vector a, fp_vector b) {
	auto repeated_mask = _mm256_castsi256_ps(_mm256_set1_epi32(mask));
	const auto mask_filter = _mm256_castsi256_ps(_mm256_setr_epi32(
		0x00000001, 0x00000002, 0x00000004, 0x00000008,
		0x00000010, 0x00000020, 0x00000040, 0x00000080));
	//auto filtered_bits = _mm256_and_ps(repeated_mask, mask_filter);
	auto fp_mask = _mm256_cmp_ps(_mm256_and_ps(repeated_mask, mask_filter), _mm256_setzero_ps(), _CMP_NEQ_OQ);
	return _mm256_blendv_ps(b, a, fp_mask);
}
__forceinline int32_t compress_mask(fp_vector mask) {
	return _mm256_movemask_ps(mask);
}
__forceinline fp_vector is_non_zero(int_vector i) {
	return _mm256_cmp_ps(_mm256_castsi256_ps(i), _mm256_setzero_ps(), _CMP_NEQ_OQ);
}

inline const uint32_t load_masks[16] = {
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

template<int32_t blk_index = 0>
class full_vector_operation {
protected:
	uint32_t const offset;
public:
	full_vector_operation(uint32_t o) : offset(o) {}

	constexpr static int32_t block_index = blk_index;
	constexpr static bool full_operation = true;

	__forceinline fp_vector zero() {
		return _mm256_setzero_ps();
	}
	__forceinline int_vector int_zero() {
		return _mm256_setzero_si256();
	}
	__forceinline fp_vector constant(float v) {
		return _mm256_set1_ps(v);
	}
	__forceinline int_vector constant(int32_t v) {
		return _mm256_set1_epi32(v);
	}

	__forceinline fp_vector load(float const* source) {
		return _mm256_load_ps(source + offset);
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

	__forceinline fp_vector gather_load(float const* source, __m256i indices) {
		return _mm256_setr_ps(
			source[indices.m256i_i32[0]],
			source[indices.m256i_i32[1]],
			source[indices.m256i_i32[2]],
			source[indices.m256i_i32[3]],
			source[indices.m256i_i32[4]],
			source[indices.m256i_i32[5]],
			source[indices.m256i_i32[6]],
			source[indices.m256i_i32[7]]);
	}
	__forceinline int_vector gather_load(int32_t const* source, __m256i indices) {
		return _mm256_setr_epi32(
			source[indices.m256i_i32[0]],
			source[indices.m256i_i32[1]],
			source[indices.m256i_i32[2]],
			source[indices.m256i_i32[3]],
			source[indices.m256i_i32[4]],
			source[indices.m256i_i32[5]],
			source[indices.m256i_i32[6]],
			source[indices.m256i_i32[7]]);
	}
	__forceinline fp_vector gather_masked_load(float const* source, __m256i indices, fp_vector mask, fp_vector def = _mm256_setzero_ps()) {
		return select(mask, gather_load(source, indices), def);
	}
	__forceinline int_vector gather_masked_load(int32_t const* source, __m256i indices, int_vector mask, int_vector def = _mm256_setzero_si256()) {
		return _mm256_castps_si256(select(_mm256_castsi256_ps(mask), _mm256_castsi256_ps(gather_load(source, indices)), _mm256_castsi256_ps(def)));
	}

	__forceinline void store(float* dest, fp_vector value) {
		_mm256_store_ps(dest + offset, value);
	}
	__forceinline void unaligned_store(float* dest, fp_vector value) {
		_mm256_storeu_ps(dest + offset, value);
	}

	__forceinline fp_vector stream_load(float const* source) {
		return _mm256_load_ps(source + offset);
	}
	__forceinline int_vector stream_load(int32_t const* source) {
		return _mm256_load_si256((__m256i const*)(source + offset));
	}
	__forceinline void stream_store(float* dest, fp_vector value) {
		_mm256_stream_ps(dest + offset, value);
	}

	__forceinline fp_vector partial_mask() {
		return _mm256_loadu_fp((float const*)load_masks);
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
	__forceinline int_vector apply(F const& f, int_vector arg) {
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
};

template<int32_t blk_index = 0>
class full_unaligned_vector_operation : public full_vector_operation< blk_index>{
public:
	full_unaligned_vector_operation(uint32_t o) : full_vector_operation(o) {}

	__forceinline fp_vector load(float const* source) {
		return _mm256_loadu_ps(source + offset);
	}
	__forceinline int_vector load(int32_t const* source) {
		return _mm256_loadu_si256((__m256i const*)(source + offset));
	}
	__forceinline void store(float* dest, fp_vector value) {
		_mm256_storeu_ps(dest + offset, value);
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
	constexpr static bool full_operation = false;

	partial_vector_operation(uint32_t o, uint32_t c) : full_vector_operation(o), count(c) {}

	__forceinline fp_vector load(float const* source) {
		int_vector_internal mask = _mm256_loadu_si256((__m256i const*)(load_masks + 8ui32 - count));
		return _mm256_maskload_ps(source + offset, mask);
	}
	__forceinline int_vector load(int32_t const* source) {
		int_vector_internal mask = _mm256_loadu_si256((__m256i const*)(load_masks + 8ui32 - count));
		return _mm256_castps_si256(_mm256_maskload_ps((float const*)(source + offset), mask));
	}
	__forceinline void store(float* dest, fp_vector value) {
		int_vector_internal mask = _mm256_loadu_si256((__m256i const*)(load_masks + 8ui32 - count));
		_mm256_maskstore_ps(dest + offset, mask, value);
	}
	__forceinline fp_vector partial_mask() {
		return _mm256_loadu_fp((float const*)(load_masks + 8ui32 - count));
	}

	__forceinline fp_vector unaligned_load(float const* source) {
		return load(source);
	}
	__forceinline int_vector unaligned_load(int32_t const* source) {
		return load(source);
	}


	__forceinline fp_vector gather_load(float const* source, __m256i indices) {
		return _mm256_setr_ps(
			count > 0 ? source[indices.m256i_i32[0]] : 0.0f,
			count > 1 ? source[indices.m256i_i32[1]] : 0.0f,
			count > 2 ? source[indices.m256i_i32[2]] : 0.0f,
			count > 3 ? source[indices.m256i_i32[3]] : 0.0f,
			count > 4 ? source[indices.m256i_i32[4]] : 0.0f,
			count > 5 ? source[indices.m256i_i32[5]] : 0.0f,
			count > 6 ? source[indices.m256i_i32[6]] : 0.0f,
			count > 7 ? source[indices.m256i_i32[7]] : 0.0f);
	}
	__forceinline int_vector gather_load(int32_t const* source, __m256i indices) {
		return _mm256_setr_epi32(
			count > 0 ? source[indices.m256i_i32[0]] : 0,
			count > 1 ? source[indices.m256i_i32[1]] : 0,
			count > 2 ? source[indices.m256i_i32[2]] : 0,
			count > 3 ? source[indices.m256i_i32[3]] : 0,
			count > 4 ? source[indices.m256i_i32[4]] : 0,
			count > 5 ? source[indices.m256i_i32[5]] : 0,
			count > 6 ? source[indices.m256i_i32[6]] : 0,
			count > 7 ? source[indices.m256i_i32[7]] : 0);
	}
	__forceinline fp_vector gather_masked_load(float const* source, __m256i indices, fp_vector mask, fp_vector def = _mm256_setzero_ps()) {
		return select(mask, gather_load(source, indices), def);
	}
	__forceinline int_vector gather_masked_load(int32_t const* source, __m256i indices, int_vector mask, int_vector def = _mm256_setzero_si256()) {
		return _mm256_castps_si256(select(_mm256_castsi256_ps(mask), _mm256_castsi256_ps(gather_load(source, indices)), _mm256_castsi256_ps(def)));
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
	__forceinline void prefetch(int32_t const* source) {}
	template<int32_t cache_lines>
	__forceinline void prefetch(float const* source) {}
	template<int32_t cache_lines>
	__forceinline void nt_prefetch(int32_t const* source) {}
	template<int32_t cache_lines>
	__forceinline void nt_prefetch(float const* source) {}

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
	__forceinline int_vector apply(F const& f, int_vector arg) {
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
};

using partial_unaligned_vector_operation = partial_vector_operation;
