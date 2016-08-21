/*
* (C) 2016 Jack Lloyd
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#include <botan/auto_rng.h>
#include <botan/entropy_src.h>

#if defined(BOTAN_HAS_HMAC_DRBG)
  #include <botan/hmac_drbg.h>
#endif

#if defined(BOTAN_HAS_HMAC_RNG)
  #include <botan/hmac_rng.h>
#endif

#if defined(BOTAN_HAS_SYSTEM_RNG)
  #include <botan/system_rng.h>
#endif

namespace Botan {

AutoSeeded_RNG::AutoSeeded_RNG(RandomNumberGenerator& underlying_rng,
                               size_t max_output_before_reseed)
   {
   m_rng.reset(new BOTAN_AUTO_RNG_DRBG(MessageAuthenticationCode::create(BOTAN_AUTO_RNG_HMAC),
                                       underlying_rng,
                                       max_output_before_reseed));
   force_reseed();
   }

AutoSeeded_RNG::AutoSeeded_RNG(Entropy_Sources& entropy_sources,
                               size_t max_output_before_reseed)
   {
   m_rng.reset(new BOTAN_AUTO_RNG_DRBG(MessageAuthenticationCode::create(BOTAN_AUTO_RNG_HMAC),
                                       entropy_sources,
                                       max_output_before_reseed));
   force_reseed();
   }

AutoSeeded_RNG::AutoSeeded_RNG(RandomNumberGenerator& underlying_rng,
                               Entropy_Sources& entropy_sources,
                               size_t max_output_before_reseed)
   {
   m_rng.reset(new BOTAN_AUTO_RNG_DRBG(MessageAuthenticationCode::create(BOTAN_AUTO_RNG_HMAC),
                                       underlying_rng,
                                       entropy_sources,
                                       max_output_before_reseed));
   force_reseed();
   }

AutoSeeded_RNG::AutoSeeded_RNG(size_t max_output_before_reseed) :
#if defined(BOTAN_HAS_SYSTEM_RNG)
   AutoSeeded_RNG(system_rng(), max_output_before_reseed)
#else
   AutoSeeded_RNG(Entropy_Sources::global_sources(), max_output_before_reseed)
#endif
   {
   }

void AutoSeeded_RNG::force_reseed()
   {
   m_rng->force_reseed();
   m_rng->randomize(nullptr, 0);

   if(!m_rng->is_seeded())
      {
      throw Exception("AutoSeeded_RNG reseeding failed");
      }
   }

bool AutoSeeded_RNG::is_seeded() const
   {
   return m_rng->is_seeded();
   }

void AutoSeeded_RNG::clear()
   {
   m_rng->clear();
   }

std::string AutoSeeded_RNG::name() const
   {
   return m_rng->name();
   }

void AutoSeeded_RNG::add_entropy(const byte in[], size_t len)
   {
   m_rng->add_entropy(in, len);
   }

size_t AutoSeeded_RNG::reseed(Entropy_Sources& srcs,
                              size_t poll_bits,
                              std::chrono::milliseconds poll_timeout)
   {
   return m_rng->reseed(srcs, poll_bits, poll_timeout);
   }

void AutoSeeded_RNG::randomize(byte output[], size_t output_len)
   {
   randomize_with_ts_input(output, output_len);
   }

void AutoSeeded_RNG::randomize_with_input(byte output[], size_t output_len,
                                          const byte ad[], size_t ad_len)
   {
   m_rng->randomize_with_input(output, output_len, ad, ad_len);
   }

}
