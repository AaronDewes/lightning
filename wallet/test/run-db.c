  #include <lightningd/log.h>

static void db_test_fatal(const char *fmt, ...);
#define db_fatal db_test_fatal

static void db_log_(struct log *log UNUSED, enum log_level level UNUSED, const struct node_id *node_id UNUSED, bool call_notifier UNUSED, const char *fmt UNUSED, ...)
{
}
#define log_ db_log_

#include "wallet/db.c"

#include "test_utils.h"

#include <common/setup.h>
#include <stdio.h>
#include <unistd.h>

/* AUTOGENERATED MOCKS START */
/* Generated stub for derive_channel_id */
void derive_channel_id(struct channel_id *channel_id UNNEEDED,
		       const struct bitcoin_outpoint *outpoint UNNEEDED)
{ fprintf(stderr, "derive_channel_id called!\n"); abort(); }
/* Generated stub for fatal */
void   fatal(const char *fmt UNNEEDED, ...)
{ fprintf(stderr, "fatal called!\n"); abort(); }
/* Generated stub for fromwire_hsmd_get_channel_basepoints_reply */
bool fromwire_hsmd_get_channel_basepoints_reply(const void *p UNNEEDED, struct basepoints *basepoints UNNEEDED, struct pubkey *funding_pubkey UNNEEDED)
{ fprintf(stderr, "fromwire_hsmd_get_channel_basepoints_reply called!\n"); abort(); }
/* Generated stub for fromwire_hsmd_get_output_scriptpubkey_reply */
bool fromwire_hsmd_get_output_scriptpubkey_reply(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, u8 **script UNNEEDED)
{ fprintf(stderr, "fromwire_hsmd_get_output_scriptpubkey_reply called!\n"); abort(); }
/* Generated stub for get_channel_basepoints */
void get_channel_basepoints(struct lightningd *ld UNNEEDED,
			    const struct node_id *peer_id UNNEEDED,
			    const u64 dbid UNNEEDED,
			    struct basepoints *local_basepoints UNNEEDED,
			    struct pubkey *local_funding_pubkey UNNEEDED)
{ fprintf(stderr, "get_channel_basepoints called!\n"); abort(); }
/* Generated stub for new_log */
struct log *new_log(const tal_t *ctx UNNEEDED, struct log_book *record UNNEEDED,
		    const struct node_id *default_node_id UNNEEDED,
		    const char *fmt UNNEEDED, ...)
{ fprintf(stderr, "new_log called!\n"); abort(); }
/* Generated stub for towire_hsmd_get_channel_basepoints */
u8 *towire_hsmd_get_channel_basepoints(const tal_t *ctx UNNEEDED, const struct node_id *peerid UNNEEDED, u64 dbid UNNEEDED)
{ fprintf(stderr, "towire_hsmd_get_channel_basepoints called!\n"); abort(); }
/* Generated stub for towire_hsmd_get_output_scriptpubkey */
u8 *towire_hsmd_get_output_scriptpubkey(const tal_t *ctx UNNEEDED, u64 channel_id UNNEEDED, const struct node_id *peer_id UNNEEDED, const struct pubkey *commitment_point UNNEEDED)
{ fprintf(stderr, "towire_hsmd_get_output_scriptpubkey called!\n"); abort(); }
/* Generated stub for wire_sync_read */
u8 *wire_sync_read(const tal_t *ctx UNNEEDED, int fd UNNEEDED)
{ fprintf(stderr, "wire_sync_read called!\n"); abort(); }
/* Generated stub for wire_sync_write */
bool wire_sync_write(int fd UNNEEDED, const void *msg TAKES UNNEEDED)
{ fprintf(stderr, "wire_sync_write called!\n"); abort(); }
/* AUTOGENERATED MOCKS END */

static char *db_err;
static void db_test_fatal(const char *fmt, ...)
{
	va_list ap;

	/* Fail hard if we're complaining about not being in transaction */
	assert(!strstarts(fmt, "No longer in transaction"));

	va_start(ap, fmt);
	db_err = tal_vfmt(NULL, fmt, ap);
	va_end(ap);
}

void plugin_hook_db_sync(struct db *db UNNEEDED)
{
}

static struct db *create_test_db(void)
{
	struct db *db;
	char *dsn, filename[] = "/tmp/ldb-XXXXXX";

	int fd = mkstemp(filename);
	if (fd == -1)
		return NULL;
	close(fd);

	dsn = tal_fmt(NULL, "sqlite3://%s", filename);
	db = db_open(NULL, dsn);
	db->data_version = 0;
	tal_free(dsn);
	return db;
}

static bool test_empty_db_migrate(struct lightningd *ld)
{
	struct db *db = create_test_db();
	const struct ext_key *bip32_base = NULL;
	CHECK(db);
	db_begin_transaction(db);
	CHECK(db_get_version(db) == -1);
	db_migrate(ld, db, bip32_base);
	db_commit_transaction(db);

	db_begin_transaction(db);
	CHECK(db_get_version(db) == ARRAY_SIZE(dbmigrations) - 1);
	db_commit_transaction(db);

	tal_free(db);
	return true;
}

static bool test_primitives(void)
{
	struct db_stmt *stmt;
	struct db *db = create_test_db();
	db_err = NULL;
	db_begin_transaction(db);
	CHECK(db->in_transaction);
	db_commit_transaction(db);
	CHECK(!db->in_transaction);
	db_begin_transaction(db);
	db_commit_transaction(db);

	db_begin_transaction(db);
	stmt = db_prepare_v2(db, SQL("SELECT name FROM sqlite_master WHERE type='table';"));
	CHECK_MSG(db_exec_prepared_v2(stmt), "db_exec_prepared must succeed");
	CHECK_MSG(!db_err, "Simple correct SQL command");
	tal_free(stmt);

	stmt = db_prepare_v2(db, SQL("not a valid SQL statement"));
	CHECK_MSG(!db_exec_prepared_v2(stmt), "db_exec_prepared must fail");
	CHECK_MSG(db_err, "Failing SQL command");
	tal_free(stmt);
	db_err = tal_free(db_err);

	/* We didn't migrate the DB, so don't have the vars table. Pretend we
	 * didn't change anything so we don't bump the data_version. */
	db->dirty = false;
	db_commit_transaction(db);
	CHECK(!db->in_transaction);
	tal_free(db);

	return true;
}

static bool test_vars(struct lightningd *ld)
{
	struct db *db = create_test_db();
	char *varname = "testvar";
	const struct ext_key *bip32_base = NULL;
	CHECK(db);

	db_begin_transaction(db);
	db_migrate(ld, db, bip32_base);
	/* Check default behavior */
	CHECK(db_get_intvar(db, varname, 42) == 42);

	/* Check setting and getting */
	db_set_intvar(db, varname, 1);
	CHECK(db_get_intvar(db, varname, 42) == 1);

	/* Check updating */
	db_set_intvar(db, varname, 2);
	CHECK(db_get_intvar(db, varname, 42) == 2);
	db_commit_transaction(db);

	tal_free(db);
	return true;
}

static bool test_manip_columns(void)
{
	struct db_stmt *stmt;
	struct db *db = create_test_db();
	const char *field1 = "field1";

	db_begin_transaction(db);
	/* tablea refers to tableb */
	stmt = db_prepare_v2(db, SQL("CREATE TABLE tablea ("
				     "  id BIGSERIAL"
				     ", field1 INTEGER"
				     ", PRIMARY KEY (id))"));
	CHECK_MSG(db_exec_prepared_v2(stmt), "db_exec_prepared must succeed");
	CHECK_MSG(!db_err, "Simple correct SQL command");
	tal_free(stmt);

	stmt = db_prepare_v2(db, SQL("INSERT INTO tablea (id, field1) VALUES (0, 1);"));
	CHECK_MSG(db_exec_prepared_v2(stmt), "db_exec_prepared must succeed");
	CHECK_MSG(!db_err, "Simple correct SQL command");
	tal_free(stmt);

	stmt = db_prepare_v2(db, SQL("CREATE TABLE tableb ("
				     "   id REFERENCES tablea(id) ON DELETE CASCADE"
				     ",  field1 INTEGER"
				     ",  field2 INTEGER);"));
	CHECK_MSG(db_exec_prepared_v2(stmt), "db_exec_prepared must succeed");
	CHECK_MSG(!db_err, "Simple correct SQL command");
	tal_free(stmt);

	stmt = db_prepare_v2(db, SQL("INSERT INTO tableb (id, field1, field2) VALUES (0, 1, 2);"));
	CHECK_MSG(db_exec_prepared_v2(stmt), "db_exec_prepared must succeed");
	CHECK_MSG(!db_err, "Simple correct SQL command");
	tal_free(stmt);
	/* Don't let it try to set a version field (we don't have one!) */
	db->dirty = false;
	db->changes = tal_arr(db, const char *, 0);
	db_commit_transaction(db);

	/* Rename tablea.field1 -> table1.field1a. */
	CHECK(db->config->rename_column(db, "tablea", "field1", "field1a"));
	/* Remove tableb.field1. */
	CHECK(db->config->delete_columns(db, "tableb", &field1, 1));

	db_begin_transaction(db);
	stmt = db_prepare_v2(db, SQL("SELECT id, field1a FROM tablea;"));
	CHECK_MSG(db_query_prepared(stmt), "db_query_prepared must succeed");
	CHECK_MSG(!db_err, "Simple correct SQL command");
	CHECK(db_step(stmt));
	CHECK(db_col_u64(stmt, "id") == 0);
	CHECK(db_col_u64(stmt, "field1a") == 1);
	CHECK(!db_step(stmt));
	tal_free(stmt);

	stmt = db_prepare_v2(db, SQL("SELECT id, field2 FROM tableb;"));
	CHECK_MSG(db_query_prepared(stmt), "db_query_prepared must succeed");
	CHECK_MSG(!db_err, "Simple correct SQL command");
	CHECK(db_step(stmt));
	CHECK(db_col_u64(stmt, "id") == 0);
	CHECK(db_col_u64(stmt, "field2") == 2);
	CHECK(!db_step(stmt));
	tal_free(stmt);
	db->dirty = false;
	db->changes = tal_arr(db, const char *, 0);
	db_commit_transaction(db);

	db_begin_transaction(db);
	/* This will actually fail */
	stmt = db_prepare_v2(db, SQL("SELECT field1 FROM tablea;"));
	CHECK_MSG(!db_query_prepared(stmt), "db_query_prepared must fail");
	db->dirty = false;
	db->changes = tal_arr(db, const char *, 0);
	db_commit_transaction(db);

	db_begin_transaction(db);
	/* This will actually fail */
	stmt = db_prepare_v2(db, SQL("SELECT field1 FROM tableb;"));
	CHECK_MSG(!db_query_prepared(stmt), "db_query_prepared must fail");
	db->dirty = false;
	db->changes = tal_arr(db, const char *, 0);
	db_commit_transaction(db);

	tal_free(db);
	return true;
}

int main(int argc, char *argv[])
{
	bool ok = true;
	/* Dummy for migration hooks */
	struct lightningd *ld = tal(NULL, struct lightningd);

	common_setup(argv[0]);
	ld->config = test_config;

	ok &= test_empty_db_migrate(ld);
	ok &= test_vars(ld);
	ok &= test_primitives();
	ok &= test_manip_columns();

	tal_free(ld);
	common_shutdown();
	return !ok;
}
