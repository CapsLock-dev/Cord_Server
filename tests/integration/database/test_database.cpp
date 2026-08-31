#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "database/database.hpp"

namespace db = cl::database;

static std::unique_ptr<db::Database> s_db;

class DatabaseTest : public testing::Test {
    protected:
    static void SetUpTestSuite() {
        std::string conn_str =
            "dbname=test_db user=test_user password=test_pass host=127.0.0.1 "
            "port=5433";
        s_db = std::make_unique<db::Database>(conn_str, 1);
    }
    static void TearDownTestSuite() {
        s_db.reset();
    }
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(DatabaseTest, TransactionSuccess) {
    std::string uuid = "cfe9ba13-5658-47aa-80f7-ee320b9bef95";
    std::string username = "username_test_1";
    std::string hash = "hashhahsahash";
    auto lambda = [&username, &uuid, &hash](pqxx::work& txn) -> auto {
        auto user_search = txn.exec("SELECT id FROM users WHERE username=$1",
                                    pqxx::params(username));
        if (!user_search.empty()) {
            txn.abort();
            return "user exists";
        }
        auto user_res = txn.exec(
            "INSERT INTO users (username, password_hash) VALUES ($1, "
            "'hashashahs') RETURNING id",
            pqxx::params(username));
        int user_id = user_res.one_field().as<int>();
        auto token_res = txn.exec(
            "INSERT INTO tokens (owner_id, token_hash, device_uuid, "
            "expires_at) VALUES ($1, $2, $3, NOW()+INTERVAL'7 days')",
            pqxx::params(user_id, hash, uuid));
        txn.commit();
        return "good";
    };
    auto res = s_db->transaction(lambda);
    EXPECT_TRUE(res.has_value()) << res.error().detail;
    EXPECT_EQ(res, "good") << res.value();

    res = s_db->transaction(lambda);
    EXPECT_TRUE(res.has_value()) << res.error().detail;
    EXPECT_EQ(res, "user exists") << res.value();
}

TEST_F(DatabaseTest, TransactionAbortOnFail) {
    std::string uuid = "cfe9ba13-5658-47aa-80f7-ee320b9bef95";
    std::string username = "username_test_1";
    std::string hash = "hashhahsahash";
    auto lambda = [&username, &uuid, &hash](pqxx::work& txn) -> auto {
        auto user_search = txn.exec("SELECT id FROM users WHERE username=$1",
                                    pqxx::params(username));
        if (!user_search.empty()) {
            txn.abort();
            return "user exists";
        }
        auto user_res = txn.exec(
            "INSERT INTO users (username, password_hash) VALUES ($1, "
            "'hashashahs') RETURNING id",
            pqxx::params(username));
        int user_id = user_res.one_field().as<int>();
        auto token_res = txn.exec(
            "INSERT INTO tokens (owner_id, token_hash, device_uuid, "
            "expires_at) VALUES ($1, $2, $3, NOW()+INTERVAL'7 days')",
            pqxx::params(user_id, hash, uuid));
        txn.commit();
        return "good";
    };
    username = "username_test_2";
    auto res = s_db->transaction(lambda);
    EXPECT_FALSE(res.has_value()) << res.value();
    EXPECT_EQ(res.error().ec, db::DatabaseErrorCode::ConstraintViolation);

    username = "username_test_2";
    hash = "hashashash";
    res = s_db->transaction(lambda);
    EXPECT_TRUE(res.has_value()) << res.error().detail;
    EXPECT_EQ(res.value(), "good");
}

TEST_F(DatabaseTest, Preset) {
    std::vector<std::pair<std::string_view, std::string_view>> statements = {
        {"get_users", "SELECT id FROM users WHERE username=$1"},
        {"insert_user",
         "INSERT INTO users (username, password_hash) VALUES ($1, $2) "
         "RETURNING id"},
        {"insert_token",
         "INSERT INTO tokens (owner_id, token_hash, device_uuid, expires_at) "
         "VALUES ($1, 'hashashahs', 'cfe9ba13-5658-47aa-80f7-ee320b9bef95', "
         "NOW()+INTERVAL'7 days')"}};
    s_db->prepare_statements(statements);
    auto lambda = [](pqxx::work& txn) -> auto {
        auto user_search =
            txn.exec(pqxx::prepped("get_users"), pqxx::params("test_name"));
        if (!user_search.empty()) {
            txn.abort();
            return "user exists";
        }
        auto user_res = txn.exec(pqxx::prepped("insert_user"),
                                 pqxx::params("test_name", "hahahash"));
        int user_id = user_res.one_field().as<int>();
        auto token_res = txn.exec(pqxx::prepped("insert_token"), user_id);
        txn.commit();
        return "good";
    };
    auto res = s_db->transaction(lambda);
    EXPECT_TRUE(res.has_value()) << res.error().detail;
    EXPECT_EQ(res, "good") << res.value();
}
