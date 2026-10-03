# TPC-Havoc query coverage

`queries.inc` contains all 220 query labels imported from
[BenchBox](https://github.com/BenchBox-dev/BenchBox/tree/0a5b88a35946bb0bd86181ae41e7595d8d54d9fb/benchbox/core/tpchavoc/variant_sets),
pinned to the revision used in the report on
[Sirius issue #1977](https://github.com/sirius-db/sirius/issues/1977).
The SQL and descriptions are preserved under the upstream MIT license.
Q19–Q22 repeat their SQL across variants; all labels are retained for comparison
with the report, which counted 184 distinct normalized statements.

## Run

The cases reuse the read-only `data/duckdb/integration.duckdb` TPC-H fixture.
Override it with `SIRIUS_INTEGRATION_TEST_DB_PATH` if needed. No download,
BenchBox installation, or YAML parser is needed to run the tests.

```bash
pixi run build/release/extension/sirius/test/cpp/sirius_unittest "[tpch_havoc]"
pixi run build/release/extension/sirius/test/cpp/sirius_unittest "TPC-Havoc Q11_V07"
```

Every label is a separate Catch2 case. Cases require GPU execution with fallback
disabled and compare complete results against DuckDB CPU using the shared exact,
order-insensitive multiset comparator (duplicate rows and NULLs are retained).
This initial suite does not validate output ordering or apply numeric tolerances;
floating-point reduction differences can therefore fail the exact comparison.

All cases are hidden from the default `make test` run pending a current GPU
baseline. Explicitly selecting `[tpch_havoc]` runs them, and unsupported queries
and known bugs fail normally; no failures are suppressed. Promote verified cases
into default coverage and introduce documented, query-specific expectations and
tolerances after establishing that baseline. A crash can interrupt the corpus
run; use individual query names to isolate it.

## Regenerate

The importer needs Python and PyYAML and fetches only the pinned upstream files:

```bash
pixi run python scripts/import_tpch_havoc.py
pixi run python scripts/import_tpch_havoc.py --check
```

For an offline import or check, add `--source-dir /path/to/variant_sets`, pointing
to `q01.yaml` through `q22.yaml` from the pinned revision. Regeneration validates
exactly ten unique variant IDs per base query and keeps their original SQL.
