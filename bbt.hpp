#ifndef BBT_H
#define BBT_H

#include <vector>
#include <tuple>
#include <ostream>
#include <stdexcept>
#include <cassert>
#include <cmath>

namespace bbt {
    struct Rating {
        explicit Rating(double max_points = 50.0) {
            m_mu = max_points / 2.0;
            m_sigma = max_points / 3.0;
            m_sigma_sq = std::pow(m_sigma, 2.0);
        }

        Rating(double mu, double sigma) : m_mu(mu), m_sigma(sigma) {
            m_sigma_sq = std::pow(m_sigma, 2.0);
        }

        double mu() const {
            return m_mu;
        }

        double sigma() const {
            return m_sigma;
        }

        friend class Rater;

        friend std::ostream &operator<<(std::ostream &os, const Rating &r) {
            os << "Rating(mu: " << r.m_mu << ", sigma: " << r.m_sigma << ", sigma_sq: " << r.m_sigma_sq << ')';
            return os;
        }

    private:
        Rating(double mu, double sigma, double sigma_sq) : m_mu(mu), m_sigma(sigma), m_sigma_sq(sigma_sq) {}

        double m_mu;
        double m_sigma;
        double m_sigma_sq;
    };

    enum class Outcome {
        Win,
        Loss,
        Draw,
    };

    struct Rater {
        explicit Rater(double point_center = 25.0, double variance_factor = 4.16666666) : m_max_points(
                point_center * 2) {
            assert(variance_factor <= point_center);
            auto beta = variance_factor;
            beta_sq = beta * beta;
        }

        std::vector<std::vector<Rating>>
        update_ratings(std::vector<std::vector<Rating>> teams, std::vector<size_t> ranks) const {
            assert(teams.size() == ranks.size());

            auto const team_size = teams.size();
            auto team_mu = std::vector<double>(team_size, 0.0);
            auto team_sigma_sq = std::vector<double>(team_size, 0.0);
            auto team_omega = std::vector<double>(team_size, 0.0);
            auto team_delta = std::vector<double>(team_size, 0.0);

            ////////////////////////////////////////////////////////////////////////
            // Step 1 - Collect Team skill and variance ////////////////////////////
            ////////////////////////////////////////////////////////////////////////

            for (auto team_idx = 0; team_idx < team_size; team_idx++) {
                auto team = teams[team_idx];
                if (team.empty()) {
                    throw std::runtime_error("At least one of the teams contains no players");
                }

                for (auto player: team) {
                    team_mu[team_idx] += player.mu();
                    team_sigma_sq[team_idx] += player.m_sigma_sq;
                }
            }

            ////////////////////////////////////////////////////////////////////////
            // Step 2 - Compute Team Omega and Delta ///////////////////////////////
            ////////////////////////////////////////////////////////////////////////

            for (auto team_idx = 0; team_idx < team_size; team_idx++) {
                for (auto team2_idx = 0; team2_idx < team_size; team2_idx++) {
                    if (team_idx == team2_idx) {
                        continue;
                    }

                    auto c = std::sqrt(team_sigma_sq[team_idx] + team_sigma_sq[team2_idx] + 2.0 * beta_sq);
                    auto e1 = std::exp(team_mu[team_idx] / c);
                    auto e2 = std::exp(team_mu[team2_idx] / c);
                    auto piq = e1 / (e1 + e2);
                    auto pqi = e2 / (e1 + e2);
                    auto ri = ranks[team_idx];
                    auto rq = ranks[team2_idx];

                    auto s = 0.0;
                    if (rq > ri) {
                        s = 1.0;
                    } else if (rq == ri) {
                        s = 0.5;
                    }

                    auto delta = (team_sigma_sq[team_idx] / c) * (s - piq);
                    auto gamma = std::sqrt(team_sigma_sq[team_idx]) / c;
                    auto eta = gamma * (team_sigma_sq[team_idx] / (c * c)) * piq * pqi;

                    team_omega[team_idx] += delta;
                    team_delta[team_idx] += eta;
                }
            }

            ////////////////////////////////////////////////////////////////////////
            // Step 3 - Individual skill update ////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////

            auto result = std::vector<std::vector<Rating>>{};
            for (auto team_idx = 0; team_idx < team_size; team_idx++) {
                auto team = teams[team_idx];
                auto team_result = std::vector<Rating>{};

                for (auto player: team) {
                    auto new_mu = player.m_mu + (player.m_sigma_sq / team_sigma_sq[team_idx]) * team_omega[team_idx];
                    if (new_mu < 0.0) {
                        new_mu = 0.0;
                    }

                    if (new_mu > m_max_points) {
                        new_mu = m_max_points;
                    }

                    auto sigma_adj = 1.0 - (player.m_sigma_sq / team_sigma_sq[team_idx]) * team_delta[team_idx];
                    if (sigma_adj < 0.0001) {
                        sigma_adj = 0.0001;
                    }

                    auto new_sigma_sq = player.m_sigma_sq * sigma_adj;

                    team_result.push_back(Rating(new_mu, std::sqrt(new_sigma_sq), new_sigma_sq));
                }

                result.push_back(team_result);
            }

            return result;
        }

        std::tuple<Rating, Rating> duel(Rating p1, Rating p2, Outcome outcome) const {
            auto teams = std::vector<std::vector<Rating>>{{p1},
                                                          {p2}};
            std::vector<size_t> ranks;
            switch (outcome) {
                case Outcome::Win:
                    ranks = {1, 2};
                    break;
                case Outcome::Loss:
                    ranks = {2, 1};
                    break;
                case Outcome::Draw:
                    ranks = {1, 1};
                    break;
            }

            auto result = update_ratings(teams, ranks);
            return std::make_tuple(result[0][0], result[1][0]);
        }

    private:
        double beta_sq;
        double m_max_points;
    };

}

#endif //BBT_H