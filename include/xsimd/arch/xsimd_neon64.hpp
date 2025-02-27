#ifndef XSIMD_NEON64_HPP
#define XSIMD_NEON64_HPP

#include "../types/xsimd_neon64_register.hpp"
#include "../types/xsimd_utils.hpp"

namespace xsimd
{
    namespace kernel
    {
        using namespace types;

        /*************
         * broadcast *
         *************/

        // Required to avoid ambiguous call
        template <class A, class T>
        batch<T, A> broadcast(T val, requires_arch<neon64>)
        {
            return broadcast<neon64>(val, neon{});
        }

        template <class A>
        batch<double, A> broadcast(double val, requires_arch<neon64>)
        {
            return vdupq_n_f64(val);
        }

        /*******
         * set *
         *******/

        template <class A>
        batch<double, A> set(batch<double, A> const&, requires_arch<neon64>, double d0, double d1)
        {
            return float64x2_t{d0, d1};
        }

        template <class A>
        batch_bool<double, A> set(batch_bool<double, A> const&, requires_arch<neon64>, bool b0, bool b1)
        {
            using register_type = typename batch_bool<double, A>::register_type;
            using unsigned_type = as_unsigned_integer_t<double>;
            return register_type{static_cast<unsigned_type>(b0 ? -1LL : 0LL),
                                 static_cast<unsigned_type>(b1 ? -1LL : 0LL)};
        }

        /*************
         * from_bool *
         *************/

        template <class A>
        batch<double, A> from_bool(batch_bool<double, A> const& arg, requires_arch<neon64>)
        {
            return vreinterpretq_f64_u64(vandq_u64(arg, vreinterpretq_u64_f64(vdupq_n_f64(1.))));
        }

        /********
         * load *
         ********/

        template <class A>
        batch<double, A> load_aligned(double const* src, convert<double>, requires_arch<neon64>)
        {
            return vld1q_f64(src);
        }

        template <class A>
        batch<double, A> load_unaligned(double const* src, convert<double>, requires_arch<neon64>)
        {
            return load_aligned<A>(src, convert<double>(), A{});
        }

        /*********
         * store *
         *********/

        template <class A>
        void store_aligned(double* dst, batch<double, A> const& src, requires_arch<neon64>)
        {
            vst1q_f64(dst, src);
        }

        template <class A>
        void store_unaligned(double* dst, batch<double, A> const& src, requires_arch<neon64>)
        {
            return store_aligned<A>(dst, src, A{});
        }

        /****************
         * load_complex *
         ****************/

        template <class A>
        batch<std::complex<double>, A> load_complex_aligned(std::complex<double> const* mem, requires_arch<neon64>)
        {
            using real_batch = batch<double, A>;
            const double* buf = reinterpret_cast<const double*>(mem);
            float64x2x2_t tmp = vld2q_f64(buf);
            real_batch real = tmp.val[0],
                       imag = tmp.val[1];
            return batch<std::complex<double>, A>{real, imag};
        }

        template <class A>
        batch<std::complex<double>, A> load_complex_unaligned(std::complex<double> const* mem, requires_arch<neon64>)
        {
            return load_complex_aligned<A>(mem, A{});
        }

        /*****************
         * store_complex *
         *****************/

        template <class A>
        void store_complex_aligned(std::complex<double>* dst, batch<std::complex<double> ,A> const& src, requires_arch<neon64>)
        {
            float64x2x2_t tmp;
            tmp.val[0] = src.real();
            tmp.val[1] = src.imag();
            double* buf = reinterpret_cast<double*>(dst);
            vst2q_f64(buf, tmp);
        }

        template <class A>
        void store_complex_unaligned(std::complex<double>* dst, batch<std::complex<double>, A> const& src, requires_arch<neon64>)
        {
            store_complex_aligned(dst, src, A{});
        }

        /*******
         * neg *
         *******/

        template <class A, class T,  detail::enable_sized_unsigned_t<T, 8> = 0>
        batch<T, A> neg(batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vreinterpretq_u64_s64(vnegq_s64(vreinterpretq_s64_u64(rhs)));
        }

        template <class A, class T,  detail::enable_sized_signed_t<T, 8> = 0>
        batch<T, A> neg(batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vnegq_s64(rhs);
        }

        template <class A>
        batch<double, A> neg(batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vnegq_f64(rhs);
        }

        /*******
         * add *
         *******/

        template <class A>
        batch<double, A> add(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vaddq_f64(lhs, rhs);
        }

        /********
         * sadd *
         ********/

        template <class A>
        batch<double, A> sadd(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return add(lhs, rhs, neon64{});
        }

        /*******
         * sub *
         *******/

        template <class A>
        batch<double, A> sub(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vsubq_f64(lhs, rhs);
        }

        /********
         * ssub *
         ********/

        template <class A>
        batch<double, A> ssub(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return sub(lhs, rhs, neon64{});
        }

        /*******
         * mul *
         *******/

        template <class A>
        batch<double, A> mul(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vmulq_f64(lhs, rhs);
        }

        /*******
         * div *
         *******/

#if defined(XSIMD_FAST_INTEGER_DIVISION)
        template <class A, class T,  detail::enable_sized_unsigned_t<T, 8> = 0>
        batch<T, A> div(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vcvtq_u64_f64(vcvtq_f64_u64(lhs) / vcvtq_f64_u64(rhs));
        }

        template <class A, class T,  detail::enable_sized_signed_t<T, 8> = 0>
        batch<T, A> div(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vcvtq_s64_f64(vcvtq_f64_s64(lhs) / vcvtq_f64_s64(rhs));
        }
#endif
        template <class A>
        batch<double, A> div(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vdivq_f64(lhs, rhs);
        }

        /******
         * eq *
         ******/

        template <class A, class T,  detail::enable_sized_unsigned_t<T, 8> = 0>
        batch_bool<T, A> eq(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vceqq_u64(lhs, rhs);
        }

        template <class A, class T,  detail::enable_sized_signed_t<T, 8> = 0>
        batch_bool<T, A> eq(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vceqq_s64(lhs, rhs);
        }

        template <class A>
        batch_bool<double, A> eq(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vceqq_f64(lhs, rhs);
        }

        template <class A, class T,  detail::enable_sized_unsigned_t<T, 8> = 0>
        batch_bool<T, A> eq(batch_bool<T, A> const& lhs, batch_bool<T, A> const& rhs, requires_arch<neon64>)
        {
            return vceqq_u64(lhs, rhs);
        }

        template <class A, class T,  detail::enable_sized_signed_t<T, 8> = 0>
        batch_bool<T, A> eq(batch_bool<T, A> const& lhs, batch_bool<T, A> const& rhs, requires_arch<neon64>)
        {
            return vceqq_u64(lhs, rhs);
        }

        template <class A>
        batch_bool<double, A> eq(batch_bool<double, A> const& lhs, batch_bool<double, A> const& rhs, requires_arch<neon64>)
        {
            return vceqq_u64(lhs, rhs);
        }

        /******
         * lt *
         ******/

        template <class A, class T,  detail::enable_sized_unsigned_t<T, 8> = 0>
        batch_bool<T, A> lt(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vcltq_u64(lhs, rhs);
        }

        template <class A, class T,  detail::enable_sized_signed_t<T, 8> = 0>
        batch_bool<T, A> lt(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vcltq_s64(lhs, rhs);
        }

        template <class A>
        batch_bool<double, A> lt(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vcltq_f64(lhs, rhs);
        }

        /******
         * le *
         ******/
        
        template <class A, class T,  detail::enable_sized_unsigned_t<T, 8> = 0>
        batch_bool<T, A> le(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vcleq_u64(lhs, rhs);
        }

        template <class A, class T,  detail::enable_sized_signed_t<T, 8> = 0>
        batch_bool<T, A> le(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vcleq_s64(lhs, rhs);
        }

        template <class A>
        batch_bool<double, A> le(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vcleq_f64(lhs, rhs);
        }

        /******
         * gt *
         ******/

        template <class A, class T,  detail::enable_sized_unsigned_t<T, 8> = 0>
        batch_bool<T, A> gt(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vcgtq_u64(lhs, rhs);
        }

        template <class A, class T,  detail::enable_sized_signed_t<T, 8> = 0>
        batch_bool<T, A> gt(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vcgtq_s64(lhs, rhs);
        }

        template <class A>
        batch_bool<double, A> gt(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vcgtq_f64(lhs, rhs);
        }

        /******
         * ge *
         ******/

        template <class A, class T,  detail::enable_sized_unsigned_t<T, 8> = 0>
        batch_bool<T, A> ge(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vcgeq_u64(lhs, rhs);
        }

        template <class A, class T,  detail::enable_sized_signed_t<T, 8> = 0>
        batch_bool<T, A> ge(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vcgeq_s64(lhs, rhs);
        }

        template <class A>
        batch_bool<double, A> ge(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vcgeq_f64(lhs, rhs);
        }

        /***************
         * bitwise_and *
         ***************/

        template <class A>
        batch<double, A> bitwise_and(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vreinterpretq_f64_u64(vandq_u64(vreinterpretq_u64_f64(lhs),
                                                   vreinterpretq_u64_f64(rhs)));
        }

        template <class A>
        batch_bool<double, A> bitwise_and(batch_bool<double, A> const& lhs, batch_bool<double, A> const& rhs, requires_arch<neon64>)
        {
            return vandq_u64(lhs, rhs);
        }

        /**************
         * bitwise_or *
         **************/

        template <class A>
        batch<double, A> bitwise_or(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vreinterpretq_f64_u64(vorrq_u64(vreinterpretq_u64_f64(lhs),
                                                   vreinterpretq_u64_f64(rhs)));
        }

        template <class A>
        batch_bool<double, A> bitwise_or(batch_bool<double, A> const& lhs, batch_bool<double, A> const& rhs, requires_arch<neon64>)
        {
            return vorrq_u64(lhs, rhs);
        }

        /***************
         * bitwise_xor *
         ***************/

        template <class A>
        batch<double, A> bitwise_xor(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vreinterpretq_f64_u64(veorq_u64(vreinterpretq_u64_f64(lhs),
                                                   vreinterpretq_u64_f64(rhs)));
        }

        template <class A>
        batch_bool<double, A> bitwise_xor(batch_bool<double, A> const& lhs, batch_bool<double, A> const& rhs, requires_arch<neon64>)
        {
            return veorq_u64(lhs, rhs);
        }

        /*******
         * neq *
         *******/

        template <class A>
        batch_bool<double, A> neq(batch_bool<double, A> const& lhs, batch_bool<double, A> const& rhs, requires_arch<neon64>)
        {
            return bitwise_xor(lhs, rhs, A{});
        }

        /***************
         * bitwise_not *
         ***************/

        template <class A>
        batch<double, A> bitwise_not(batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vreinterpretq_f64_u32(vmvnq_u32(vreinterpretq_u32_f64(rhs)));
        }

        template <class A>
        batch_bool<double, A> bitwise_not(batch_bool<double, A> const& rhs, requires_arch<neon64>)
        {
            return detail::bitwise_not_u64(rhs);
        }

        /******************
         * bitwise_andnot *
         ******************/

        template <class A>
        batch<double, A> bitwise_andnot(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vreinterpretq_f64_u64(vbicq_u64(vreinterpretq_u64_f64(lhs),
                                                   vreinterpretq_u64_f64(rhs)));
        }
        
        template <class A>
        batch_bool<double, A> bitwise_andnot(batch_bool<double, A> const& lhs, batch_bool<double, A> const& rhs, requires_arch<neon64>)
        {
            return vbicq_u64(lhs, rhs);
        }

        /*******
         * min *
         *******/

        template <class A>
        batch<double, A> min(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vminq_f64(lhs, rhs);
        }

        /*******
         * max *
         *******/

        template <class A>
        batch<double, A> max(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vmaxq_f64(lhs, rhs);
        }

        /*******
         * abs *
         *******/

        template <class A, class T,  detail::enable_sized_unsigned_t<T, 8> = 0>
        batch<T, A> abs(batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return rhs;
        }

        template <class A, class T,  detail::enable_sized_signed_t<T, 8> = 0>
        batch<T, A> abs(batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vabsq_s64(rhs);
        }

        template <class A>
        batch<double, A> abs(batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vabsq_f64(rhs);
        }

        /********
         * sqrt *
         ********/

        template <class A>
        batch<double, A> sqrt(batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vsqrtq_f64(rhs);
        }

        /********************
         * Fused operations *
         ********************/
        
#ifdef __ARM_FEATURE_FMA
        template <class A>
        batch<double, A> fma(batch<double, A> const& x, batch<double, A> const& y, batch<double, A> const& z, requires_arch<neon64>)
        {
            return vfmaq_f64(z, x, y);
        }

        template <class A>
        batch<double, A> fms(batch<double, A> const& x, batch<double, A> const& y, batch<double, A> const& z, requires_arch<neon64>)
        {
            return vfmaq_f64(-z, x, y);
        }
#endif

        /********
         * hadd *
         ********/

        template <class A, class T, detail::enable_sized_unsigned_t<T, 1> = 0>
        typename batch<T, A>::value_type hadd(batch<T, A> const& arg, requires_arch<neon64>)
        {
            return vaddvq_u8(arg);
        }

        template <class A, class T, detail::enable_sized_signed_t<T, 1> = 0>
        typename batch<T, A>::value_type hadd(batch<T, A> const& arg, requires_arch<neon64>)
        {
            return vaddvq_s8(arg);
        }

        template <class A, class T, detail::enable_sized_unsigned_t<T, 2> = 0>
        typename batch<T, A>::value_type hadd(batch<T, A> const& arg, requires_arch<neon64>)
        {
            return vaddvq_u16(arg);
        }

        template <class A, class T, detail::enable_sized_signed_t<T, 2> = 0>
        typename batch<T, A>::value_type hadd(batch<T, A> const& arg, requires_arch<neon64>)
        {
            return vaddvq_s16(arg);
        }

        template <class A, class T, detail::enable_sized_unsigned_t<T, 4> = 0>
        typename batch<T, A>::value_type hadd(batch<T, A> const& arg, requires_arch<neon64>)
        {
            return vaddvq_u32(arg);
        }

        template <class A, class T, detail::enable_sized_signed_t<T, 4> = 0>
        typename batch<T, A>::value_type hadd(batch<T, A> const& arg, requires_arch<neon64>)
        {
            return vaddvq_s32(arg);
        }

        template <class A, class T, detail::enable_sized_unsigned_t<T, 8> = 0>
        typename batch<T, A>::value_type hadd(batch<T, A> const& arg, requires_arch<neon64>)
        {
            return vaddvq_u64(arg);
        }

        template <class A, class T, detail::enable_sized_signed_t<T, 8> = 0>
        typename batch<T, A>::value_type hadd(batch<T, A> const& arg, requires_arch<neon64>)
        {
            return vaddvq_s64(arg);
        }

        template <class A>
        double hadd(batch<double, A> const& arg, requires_arch<neon64>)
        {
            return vaddvq_f64(arg);
        }

        /*********
         * haddp *
         *********/

        template <class A>
        batch<double, A> haddp(const batch<double, A>* row, requires_arch<neon64>)
        {
            return vpaddq_f64(row[0], row[1]);
        }

        /**********
         * select *
         **********/

        template <class A>
        batch<double, A> select(batch_bool<double, A> const& cond, batch<double, A> const& a, batch<double, A> const& b, requires_arch<neon64>)
        {
            return vbslq_f64(cond, a, b);
        }

        template <class A, bool... b>
        batch<double, A> select(batch_bool_constant<batch<double, A>, b...> const&,
                                batch<double, A> const& true_br,
                                batch<double, A> const& false_br,
                                requires_arch<neon64>)
        {
            return select(batch_bool<double, A>{b...}, true_br, false_br, neon64{});
        }
        /**********
         * zip_lo *
         **********/

        template <class A, class T, detail::enable_sized_unsigned_t<T, 8> = 0>
        batch<T, A> zip_lo(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vzip1q_u64(lhs, rhs);
        }

        template <class A, class T, detail::enable_sized_signed_t<T, 8> = 0>
        batch<T, A> zip_lo(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vzip1q_s64(lhs, rhs);
        }

        template <class A>
        batch<double, A> zip_lo(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vzip1q_f64(lhs, rhs);
        }

        /**********
         * zip_hi *
         **********/

        template <class A, class T, detail::enable_sized_unsigned_t<T, 8> = 0>
        batch<T, A> zip_hi(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vzip2q_u64(lhs, rhs);
        }

        template <class A, class T, detail::enable_sized_signed_t<T, 8> = 0>
        batch<T, A> zip_hi(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vzip2q_s64(lhs, rhs);
        }

        template <class A>
        batch<double, A> zip_hi(batch<double, A> const& lhs, batch<double, A> const& rhs, requires_arch<neon64>)
        {
            return vzip2q_f64(lhs, rhs);
        }

        /****************
         * extract_pair *
         ****************/

        namespace detail
        {
            template <class A, size_t I, size_t... Is>
            batch<double, A> extract_pair(batch<double, A> const& lhs, batch<double, A> const& rhs, std::size_t n,
                                          ::xsimd::detail::index_sequence<I, Is...>)
            {
                if (n == I)
                {
                    return vextq_f64(lhs, rhs, I);
                }
                else
                {
                    return extract_pair(lhs, rhs, n, ::xsimd::detail::index_sequence<Is...>());
                }
            }
        }

        template <class A>
        batch<double, A> extract_pair(batch<double, A> const& lhs, batch<double, A> const& rhs, std::size_t n, requires_arch<neon64>)
        {
            constexpr std::size_t size = batch<double, A>::size;
            assert(0<= n && n< size && "index in bounds");
            return detail::extract_pair(lhs, rhs, n, ::xsimd::detail::make_index_sequence<size>());
        }

        /******************
         * bitwise_rshift *
         ******************/
        
        template <class A, class T, detail::enable_sized_unsigned_t<T, 8> = 0>
        batch<T, A> bitwise_rshift(batch<T, A> const& lhs, int n, requires_arch<neon64>)
        {
            return bitwise_rshift<A>(lhs, n, neon{}); 
        }

        template <class A, class T, detail::enable_sized_unsigned_t<T, 8> = 0>
        batch<T, A> bitwise_rshift(batch<T, A> const& lhs, batch<as_signed_integer_t<T>, A> const& rhs, requires_arch<neon64>)
        {
            return vshlq_u64(lhs, vnegq_s64(rhs));
        }

        template <class A, class T, detail::enable_sized_signed_t<T, 8> = 0>
        batch<T, A> bitwise_rshift(batch<T, A> const& lhs, int n, requires_arch<neon64>)
        {
            return bitwise_rshift<A>(lhs, n, neon{}); 
        }

        template <class A, class T, detail::enable_sized_signed_t<T, 8> = 0>
        batch<T, A> bitwise_rshift(batch<T, A> const& lhs, batch<T, A> const& rhs, requires_arch<neon64>)
        {
            return vshlq_s64(lhs, vnegq_s64(rhs));
        }

        /****************
         * bitwise_cast *
         ****************/

        template <class A, class T>
        batch<double, A> bitwise_cast(batch<T, A> const& arg, batch<double, A> const&, requires_arch<neon64>)
        {
            using caster_type = detail::bitwise_caster_impl<float64x2_t,
                                                            uint8x16_t, int8x16_t,
                                                            uint16x8_t, int16x8_t,
                                                            uint32x4_t, int32x4_t,
                                                            uint64x2_t, int64x2_t,
                                                            float32x4_t>;
            constexpr caster_type caster = {
                std::make_tuple(vreinterpretq_f64_u8,  vreinterpretq_f64_s8,  vreinterpretq_f64_u16, vreinterpretq_f64_s16,
                                vreinterpretq_f64_u32, vreinterpretq_f64_s32, vreinterpretq_f64_u64, vreinterpretq_f64_s64,
                                vreinterpretq_f64_f32)
            };
            using register_type = typename batch<T, A>::register_type;
            return caster.apply(register_type(arg));
        }

        namespace detail
        {
            template <class S, class... R>
            struct bitwise_caster_neon64
            {
                using container_type = std::tuple<R (*)(S)...>;
                container_type m_func;

                template <class V>
                V apply(float64x2_t rhs) const
                {
                    using func_type = V (*)(float64x2_t);
                    auto func = xsimd::detail::get<func_type>(m_func);
                    return func(rhs);
                }
            };
        }

        template <class A, class R>
        batch<R, A> bitwise_cast(batch<double, A> const& arg, batch<R, A> const&, requires_arch<neon64>)
        {
            using caster_type = detail::bitwise_caster_neon64<float64x2_t,
                                                              uint8x16_t, int8x16_t,
                                                              uint16x8_t, int16x8_t,
                                                              uint32x4_t, int32x4_t,
                                                              uint64x2_t, int64x2_t,
                                                              float32x4_t>;
            constexpr caster_type caster = {
                std::make_tuple(vreinterpretq_u8_f64,  vreinterpretq_s8_f64,  vreinterpretq_u16_f64, vreinterpretq_s16_f64,
                                vreinterpretq_u32_f64, vreinterpretq_s32_f64, vreinterpretq_u64_f64, vreinterpretq_s64_f64,
                                vreinterpretq_f32_f64)
            };
            using src_register_type = typename batch<double, A>::register_type;
            using dst_register_type = typename batch<R, A>::register_type;
            return caster.apply<dst_register_type>(src_register_type(arg));
        }

        template <class A>
        batch<double, A> bitwise_cast(batch<double, A> const& arg, batch<double, A> const&, requires_arch<neon64>)
        {
            return arg;
        }

        /*************
         * bool_cast *
         *************/

        template <class A>
        batch_bool<double, A> bool_cast(batch_bool<int64_t, A> const& arg, requires_arch<neon64>)
        {
            using register_type = typename batch_bool<int64_t, A>::register_type;
            return register_type(arg);
        }

        template <class A>
        batch_bool<int64_t, A> bool_cast(batch_bool<double, A> const& arg, requires_arch<neon64>)
        {
            using register_type = typename batch_bool<double, A>::register_type;
            return register_type(arg);
        }

        /**********
         * to_int *
         **********/

        template <class A>
        batch<int64_t, A> to_int(const batch<double, A>& x, requires_arch<neon64>)
        {
            return vcvtq_s64_f64(x);
        }

        /************
         * to_float *
         ************/

        template <class A>
        batch<double, A> to_float(batch<int64_t, A> const& x, requires_arch<neon64>)
        {
            return vcvtq_f64_s64(x);
        }

        /*********
         * isnan *
         *********/

        template <class A>
        batch_bool<double, A> isnan(batch<double, A> const& arg, requires_arch<neon64>)
        {
            return !(arg == arg);
        }
    }
}

#endif

