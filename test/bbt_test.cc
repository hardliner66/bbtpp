#include <gtest/gtest.h>
#include <bbt.h>
#include <vector>
#include <cmath>

using namespace bbt;
using namespace std;

TEST(BBT, two_player_duel_win_loss) {
    auto p1 = Rating();
    auto p2 = Rating();

    auto rater = Rater();
    auto new_rs = rater
            .update_ratings({{p1}, {p2}}, {1, 2});

    EXPECT_LT(abs(new_rs[0][0].mu() - 27.63523138), 1.0 / 100000000.0);
    EXPECT_LT(abs(new_rs[0][0].sigma() - 8.0655063), 1.0 / 1000000.0);
    EXPECT_LT(abs(new_rs[1][0].mu() - 22.36476861), 1.0 / 100000000.0);
    EXPECT_LT(abs(new_rs[1][0].sigma() - 8.0655063), 1.0 / 1000000.0);
}

TEST(BBT, two_player_duel_tie) {
    auto p1 = Rating();
    auto p2 = Rating();

    auto rater = Rater();
    Rating new_p1, new_p2;
    std::tie(new_p1, new_p2) = rater.duel(p1, p2, Outcome::Draw);

    EXPECT_EQ(new_p1.mu(), 25.0);
    EXPECT_EQ(new_p2.mu(), 25.0);
    EXPECT_LT(abs(new_p1.sigma() - 8.0655063), 1.0 / 1000000.0);
    EXPECT_LT(abs(new_p2.sigma() - 8.0655063), 1.0 / 1000000.0);
}

TEST(BBT, four_player_race) {
    auto p1 = Rating();
    auto p2 = Rating();
    auto p3 = Rating();
    auto p4 = Rating();

    auto rater = Rater();
    auto teams = vector<vector<Rating>>{{p1}, {p2}, {p3}, {p4}};
    auto ranks = vector<size_t>{1, 2, 3, 4};

    auto new_ratings = rater.update_ratings(teams, ranks);

    EXPECT_LT(abs(new_ratings[0][0].mu() - 32.9056941), 1.0 / 10000000.0);
    EXPECT_LT(abs(new_ratings[1][0].mu() - 27.6352313), 1.0 / 10000000.0);
    EXPECT_LT(abs(new_ratings[2][0].mu() - 22.3647686), 1.0 / 10000000.0);
    EXPECT_LT(abs(new_ratings[3][0].mu() - 17.0943058), 1.0 / 10000000.0);

    EXPECT_LT(abs(new_ratings[0][0].sigma() - 7.50121906), 1.0 / 1000000.0);
    EXPECT_LT(abs(new_ratings[1][0].sigma() - 7.50121906), 1.0 / 1000000.0);
    EXPECT_LT(abs(new_ratings[2][0].sigma() - 7.50121906), 1.0 / 1000000.0);
    EXPECT_LT(abs(new_ratings[3][0].sigma() - 7.50121906), 1.0 / 1000000.0);
}
