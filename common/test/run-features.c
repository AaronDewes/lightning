#include "../features.c"
#include "../memleak.c"
#include <ccan/err/err.h>
#include <ccan/mem/mem.h>
#include <ccan/str/hex/hex.h>
#include <common/utils.h>
#include <stdio.h>
#include <wally_core.h>

/* AUTOGENERATED MOCKS START */
/* Generated stub for amount_asset_is_main */
bool amount_asset_is_main(struct amount_asset *asset UNNEEDED)
{ fprintf(stderr, "amount_asset_is_main called!\n"); abort(); }
/* Generated stub for amount_asset_to_sat */
struct amount_sat amount_asset_to_sat(struct amount_asset *asset UNNEEDED)
{ fprintf(stderr, "amount_asset_to_sat called!\n"); abort(); }
/* Generated stub for amount_sat_add */
 bool amount_sat_add(struct amount_sat *val UNNEEDED,
				       struct amount_sat a UNNEEDED,
				       struct amount_sat b UNNEEDED)
{ fprintf(stderr, "amount_sat_add called!\n"); abort(); }
/* Generated stub for amount_sat_eq */
bool amount_sat_eq(struct amount_sat a UNNEEDED, struct amount_sat b UNNEEDED)
{ fprintf(stderr, "amount_sat_eq called!\n"); abort(); }
/* Generated stub for amount_sat_sub */
 bool amount_sat_sub(struct amount_sat *val UNNEEDED,
				       struct amount_sat a UNNEEDED,
				       struct amount_sat b UNNEEDED)
{ fprintf(stderr, "amount_sat_sub called!\n"); abort(); }
/* Generated stub for fromwire_amount_sat */
struct amount_sat fromwire_amount_sat(const u8 **cursor UNNEEDED, size_t *max UNNEEDED)
{ fprintf(stderr, "fromwire_amount_sat called!\n"); abort(); }
/* Generated stub for fromwire_fail */
const void *fromwire_fail(const u8 **cursor UNNEEDED, size_t *max UNNEEDED)
{ fprintf(stderr, "fromwire_fail called!\n"); abort(); }
/* Generated stub for fromwire_secp256k1_ecdsa_signature */
void fromwire_secp256k1_ecdsa_signature(const u8 **cursor UNNEEDED, size_t *max UNNEEDED,
					secp256k1_ecdsa_signature *signature UNNEEDED)
{ fprintf(stderr, "fromwire_secp256k1_ecdsa_signature called!\n"); abort(); }
/* Generated stub for fromwire_sha256_double */
void fromwire_sha256_double(const u8 **cursor UNNEEDED, size_t *max UNNEEDED,
			    struct sha256_double *sha256d UNNEEDED)
{ fprintf(stderr, "fromwire_sha256_double called!\n"); abort(); }
/* Generated stub for fromwire_tal_arrn */
u8 *fromwire_tal_arrn(const tal_t *ctx UNNEEDED,
		       const u8 **cursor UNNEEDED, size_t *max UNNEEDED, size_t num UNNEEDED)
{ fprintf(stderr, "fromwire_tal_arrn called!\n"); abort(); }
/* Generated stub for fromwire_u16 */
u16 fromwire_u16(const u8 **cursor UNNEEDED, size_t *max UNNEEDED)
{ fprintf(stderr, "fromwire_u16 called!\n"); abort(); }
/* Generated stub for fromwire_u8 */
u8 fromwire_u8(const u8 **cursor UNNEEDED, size_t *max UNNEEDED)
{ fprintf(stderr, "fromwire_u8 called!\n"); abort(); }
/* Generated stub for towire_amount_sat */
void towire_amount_sat(u8 **pptr UNNEEDED, const struct amount_sat sat UNNEEDED)
{ fprintf(stderr, "towire_amount_sat called!\n"); abort(); }
/* Generated stub for towire_secp256k1_ecdsa_signature */
void towire_secp256k1_ecdsa_signature(u8 **pptr UNNEEDED,
			      const secp256k1_ecdsa_signature *signature UNNEEDED)
{ fprintf(stderr, "towire_secp256k1_ecdsa_signature called!\n"); abort(); }
/* Generated stub for towire_sha256_double */
void towire_sha256_double(u8 **pptr UNNEEDED, const struct sha256_double *sha256d UNNEEDED)
{ fprintf(stderr, "towire_sha256_double called!\n"); abort(); }
/* Generated stub for towire_u16 */
void towire_u16(u8 **pptr UNNEEDED, u16 v UNNEEDED)
{ fprintf(stderr, "towire_u16 called!\n"); abort(); }
/* Generated stub for towire_u8 */
void towire_u8(u8 **pptr UNNEEDED, u8 v UNNEEDED)
{ fprintf(stderr, "towire_u8 called!\n"); abort(); }
/* Generated stub for towire_u8_array */
void towire_u8_array(u8 **pptr UNNEEDED, const u8 *arr UNNEEDED, size_t num UNNEEDED)
{ fprintf(stderr, "towire_u8_array called!\n"); abort(); }
/* AUTOGENERATED MOCKS END */


static void test_featurebits_or(void)
{
	u8 *f1 = tal_arr(tmpctx, u8, 0);
	u8 *f2 = tal_arr(tmpctx, u8, 0);
	u8 *control = tal_arr(tmpctx, u8, 0);
	for (size_t i = 0; i < 100; i += 3) {
		set_feature_bit(&f1, i);
		set_feature_bit(&control, i);
	}

	for (size_t i = 0; i < 10; i += 2) {
		set_feature_bit(&f2, i);
		set_feature_bit(&control, i);
	}
	u8 *result = featurebits_or(tmpctx, take(f1), f2);
	assert(
	    memeq(result, tal_bytelen(result), control, tal_bytelen(control)));
}

static bool feature_set_eq(const struct feature_set *f1,
			   const struct feature_set *f2)
{
	/* We assume minimal sizing */
	for (size_t i = 0; i < ARRAY_SIZE(f1->bits); i++) {
		if (!memeq(f1->bits[i], tal_bytelen(f1->bits[i]),
			   f2->bits[i], tal_bytelen(f2->bits[i])))
			return false;
	}
	return true;
}

static void test_feature_set_or(void)
{
	struct feature_set *f1, *f2, *control;

	/* cppcheck-suppress uninitvar - false positive on f1->bits */
	for (size_t i = 0; i < ARRAY_SIZE(f1->bits); i++) {
		f1 = talz(tmpctx, struct feature_set);
		f2 = talz(tmpctx, struct feature_set);
		control = talz(tmpctx, struct feature_set);

		f1->bits[i] = tal_arr(f1, u8, 0);
		f2->bits[i] = tal_arr(f2, u8, 0);
		control->bits[i] = tal_arr(control, u8, 0);

		/* or with nothing is a nop */
		set_feature_bit(&f1->bits[i], 0);
		set_feature_bit(&control->bits[i], 0);
		assert(feature_set_or(f1, f2));
		assert(feature_set_eq(f1, control));

		/* or compulsory with either compulsory or optional is a fail */
		set_feature_bit(&f2->bits[i], 0);
		assert(!feature_set_or(f1, f2));
		assert(feature_set_eq(f1, control));
		assert(!feature_set_or(f2, f1));

		clear_feature_bit(f2->bits[i], 0);
		set_feature_bit(&f2->bits[i], 1);
		assert(!feature_set_or(f1, f2));
		assert(feature_set_eq(f1, control));
		assert(!feature_set_or(f2, f1));

		clear_feature_bit(f2->bits[i], 1);
		set_feature_bit(&f2->bits[i], 10);
		set_feature_bit(&control->bits[i], 10);
		assert(feature_set_or(f1, f2));
		assert(feature_set_eq(f1, control));
	}
}

int main(void)
{
	u8 *bits;
	struct feature_set *fset;

	setup_locale();
	wally_init(0);
	secp256k1_ctx = wally_get_secp_context();
	setup_tmpctx();

	/* Just some bits to set. */
	fset = feature_set_for_feature(tmpctx,
				       OPTIONAL_FEATURE(OPT_DATA_LOSS_PROTECT));

	bits = tal_arr(tmpctx, u8, 0);
	for (size_t i = 0; i < 100; i += 3)
		set_feature_bit(&bits, i);
	for (size_t i = 0; i < 100; i++)
		assert(test_bit(bits, i / 8, i % 8) == ((i % 3) == 0));

	for (size_t i = 0; i < 100; i++)
		assert(feature_is_set(bits, i) == ((i % 3) == 0));

	/* Simple test: single byte */
	bits = tal_arr(tmpctx, u8, 1);

	/* Compulsory feature */
	bits[0] = (1 << 0);
	assert(feature_offered(bits, 0));
	assert(!feature_offered(bits, 2));
	assert(!feature_offered(bits, 8));
	assert(!feature_offered(bits, 16));

	/* Optional feature */
	bits[0] = (1 << 1);
	assert(feature_offered(bits, 0));
	assert(!feature_offered(bits, 2));
	assert(!feature_offered(bits, 8));
	assert(!feature_offered(bits, 16));

	/* Endian-sensitive test: big-endian means we frob last byte here */
	bits = tal_arrz(tmpctx, u8, 2);

	bits[1] = (1 << 0);
	assert(feature_offered(bits, 0));
	assert(!feature_offered(bits, 2));
	assert(!feature_offered(bits, 8));
	assert(!feature_offered(bits, 16));

	/* Optional feature */
	bits[1] = (1 << 1);
	assert(feature_offered(bits, 0));
	assert(!feature_offered(bits, 2));
	assert(!feature_offered(bits, 8));
	assert(!feature_offered(bits, 16));

	/* We always support no features. */
	memset(bits, 0, tal_count(bits));
	assert(features_unsupported(fset, bits, INIT_FEATURE) == -1);

	/* We must support our own features. */
	assert(features_unsupported(fset, fset->bits[INIT_FEATURE], INIT_FEATURE) == -1);

	/* We can add random odd features, no problem. */
	for (size_t i = 1; i < 16; i += 2) {
		bits = tal_dup_talarr(tmpctx, u8, fset->bits[INIT_FEATURE]);
		set_feature_bit(&bits, i);
		assert(features_unsupported(fset, bits, INIT_FEATURE) == -1);
	}

	/* We can't add random even features. */
	for (size_t i = 0; i < 16; i += 2) {
		bits = tal_dup_talarr(tmpctx, u8, fset->bits[INIT_FEATURE]);
		set_feature_bit(&bits, i);

		/* Special case for missing compulsory feature */
		if (i == 2) {
			assert(features_unsupported(fset, bits, INIT_FEATURE) == i);
		} else {
			assert((features_unsupported(fset, bits, INIT_FEATURE) == -1)
			       == feature_offered(fset->bits[INIT_FEATURE], i));
		}
	}

	test_featurebits_or();
	test_feature_set_or();

	wally_cleanup(0);
	tal_free(tmpctx);
	take_cleanup();
	return 0;
}
