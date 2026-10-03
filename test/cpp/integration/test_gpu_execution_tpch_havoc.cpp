/*
 * Copyright 2026, Sirius Contributors.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <catch.hpp>
#include <duckdb.hpp>
#include <utils/gpu_execution_fixture.hpp>
#include <utils/scoped_sirius_setting.hpp>

#include <cstdlib>
#include <filesystem>
#include <string>

namespace {

class TpchHavocFixture : public sirius::test::GpuExecutionFixture {
 public:
  TpchHavocFixture()
  {
    auto const* override_path = std::getenv("SIRIUS_INTEGRATION_TEST_DB_PATH");
    auto const path           = override_path ? std::filesystem::path{override_path}
                                              : std::filesystem::path{__FILE__}.parent_path() /
                                        "data/duckdb/integration.duckdb";
    REQUIRE(std::filesystem::exists(path));
    // Escape apostrophes in an overridden path before embedding it in SQL.
    std::string escaped;
    for (char c : path.string()) {
      escaped += c;
      if (c == '\'') { escaped += c; }
    }
    run_ok("ATTACH IF NOT EXISTS '" + escaped + "' AS tpch (READ_ONLY);");
    run_ok("USE tpch;");
  }
};

}  // namespace

// Hidden until a GPU baseline classifies supported queries and known gaps.
// Each label is a separate case, so Catch2 can filter and shard the corpus.
#define HAVOC_QUERY(id, description, sql)                                                \
  TEST_CASE_METHOD(                                                                      \
    TpchHavocFixture, "TPC-Havoc " id, "[.][integration][gpu_execution][tpch_havoc]")    \
  {                                                                                      \
    INFO(description);                                                                   \
    sirius::test::scoped_sirius_setting fallback{*con, "enable_duckdb_fallback", false}; \
    sirius::test::scoped_sirius_setting execution{*con, "gpu_execution", true};          \
    compare_gpu_vs_cpu(sql);                                                             \
  }

#include "data/tpch_havoc/queries.inc"

#undef HAVOC_QUERY
