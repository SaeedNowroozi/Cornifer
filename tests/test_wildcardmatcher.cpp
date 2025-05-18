#include <gtest/gtest.h>
#include "filematcher.h"
#include "wildcardmatcher.h"
#include <QStringListModel>
#include "logger.h"
#include <filesystem>
#include <fstream>
#include "concurrentfilesearcher.h"

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        model = new QStringListModel();
        Logger::init("");
        Logger::setModel(model);
    }

    void TearDown() override {
        delete model;
    }

    QStringListModel* model;
};

TEST(WildcardMatcherTest, SimpleMatch) {
    WildcardMatcher matcher;
    matcher.addPattern("main.cpp");
    EXPECT_TRUE(matcher.matches("main.cpp"));
    EXPECT_FALSE(matcher.matches("main.c"));
}

TEST(WildcardMatcherTest, MatchWithStar) {
    WildcardMatcher matcher;
    matcher.addPattern("*.cpp");
    EXPECT_TRUE(matcher.matches("main.cpp"));
    EXPECT_TRUE(matcher.matches("test.cpp"));
    EXPECT_FALSE(matcher.matches("main.h"));
}

TEST(WildcardMatcherTest, MatchWithQuestionMark) {
    WildcardMatcher matcher;
    matcher.addPattern("file?.txt");
    EXPECT_TRUE(matcher.matches("file1.txt"));
    EXPECT_TRUE(matcher.matches("fileA.txt"));
    EXPECT_FALSE(matcher.matches("file10.txt"));
}

TEST(WildcardMatcherTest, MultiplePatterns) {
    WildcardMatcher matcher;
    matcher.addPattern("*.cpp");
    matcher.addPattern("*.h");
    EXPECT_TRUE(matcher.matches("main.cpp"));
    EXPECT_TRUE(matcher.matches("main.h"));
    EXPECT_FALSE(matcher.matches("main.txt"));
}

TEST(WildcardMatcherTest, CaseInsensitiveMatch) {
    WildcardMatcher matcher(true);
    matcher.addPattern("Main.CPP");
    EXPECT_TRUE(matcher.matches("main.cpp"));
    EXPECT_TRUE(matcher.matches("MAIN.CPP"));
    EXPECT_TRUE(matcher.matches("Main.CPP"));
    EXPECT_FALSE(matcher.matches("main.c"));
}

TEST(WildcardMatcherTest, NoPatterns) {
    WildcardMatcher matcher;
    EXPECT_FALSE(matcher.matches("main.cpp"));
}

TEST(WildcardMatcherTest, MatchWithComplexPattern) {
    WildcardMatcher matcher;
    matcher.addPattern("src/*/test?.cpp");
    EXPECT_TRUE(matcher.matches("src/module/test1.cpp"));
    EXPECT_TRUE(matcher.matches("src/utils/testA.cpp"));
    EXPECT_FALSE(matcher.matches("src/module/test10.cpp"));
    EXPECT_FALSE(matcher.matches("src/test1.cpp"));
}

TEST(FileMatcherTest, ExactMatch) {
    FileMatcher matcher({"main.cpp"});
    EXPECT_TRUE(matcher.match("main.cpp"));
    EXPECT_FALSE(matcher.match("main.h"));
}

TEST(FileMatcherTest, MatchWithStar) {
    FileMatcher matcher({"*.cpp"});
    EXPECT_TRUE(matcher.match("main.cpp"));
    EXPECT_TRUE(matcher.match("utils.cpp"));
    EXPECT_FALSE(matcher.match("main.h"));
}

TEST(FileMatcherTest, MatchWithQuestionMark) {
    FileMatcher matcher({"file?.txt"});
    EXPECT_TRUE(matcher.match("file1.txt"));
    EXPECT_TRUE(matcher.match("fileA.txt"));
    EXPECT_FALSE(matcher.match("file12.txt"));
}

TEST(FileMatcherTest, MultiplePatterns) {
    FileMatcher matcher({"*.cpp", "*.h"});
    EXPECT_TRUE(matcher.match("main.cpp"));
    EXPECT_TRUE(matcher.match("main.h"));
    EXPECT_FALSE(matcher.match("main.txt"));
}

TEST(FileMatcherTest, CaseInsensitiveMatch) {
    FileMatcher matcher({"README.md"}, true);
    EXPECT_TRUE(matcher.match("readme.md"));
    EXPECT_TRUE(matcher.match("README.MD"));
    EXPECT_FALSE(matcher.match("readme.txt"));
}

TEST(FileMatcherTest, NoPatterns) {
    FileMatcher matcher({});
    EXPECT_FALSE(matcher.match("anyfile.txt"));
}

TEST_F(LoggerTest, InfoMessageAppendsToModel) {
    Logger::info("Test info message");
    QStringList list = model->stringList();
    ASSERT_FALSE(list.isEmpty());
    EXPECT_EQ(list.last(), "Test info message");
}

TEST_F(LoggerTest, WarnMessageAppendsToModel) {
    Logger::warn("Test warn message");
    QStringList list = model->stringList();
    ASSERT_FALSE(list.isEmpty());
    EXPECT_EQ(list.last(), "[WARN] Test warn message");
}

TEST_F(LoggerTest, ErrorMessageAppendsToModel) {
    Logger::error("Test error message");
    QStringList list = model->stringList();
    ASSERT_FALSE(list.isEmpty());
    EXPECT_EQ(list.last(), "[ERROR] Test error message");
}

TEST_F(LoggerTest, DebugMessageAppendsToModel) {
    Logger::debug("Test debug message");
    QStringList list = model->stringList();
    ASSERT_FALSE(list.isEmpty());
    EXPECT_EQ(list.last(), "[DEBUG] Test debug message");
}

TEST_F(LoggerTest, LogFunctionDispatchesCorrectly) {
    Logger::log("Info level", Logger::Level::Info);
    Logger::log("Warn level", Logger::Level::Warn);
    Logger::log("Error level", Logger::Level::Error);
    Logger::log("Debug level", Logger::Level::Debug);

    QStringList list = model->stringList();
    ASSERT_EQ(list.size(), 4);
    EXPECT_EQ(list[0], "Info level");
    EXPECT_EQ(list[1], "[WARN] Warn level");
    EXPECT_EQ(list[2], "[ERROR] Error level");
    EXPECT_EQ(list[3], "[DEBUG] Debug level");
}



