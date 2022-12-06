#include <vector>
#include <iostream>
#include <memory>
#include <cstdlib>
#include <algorithm>
#include <ranges>
namespace rs = std::ranges;
namespace vw = rs::views;


///////////////////////////////////////////////////////////
///////////////////////// VIRTUAL /////////////////////////
///////////////////////////////////////////////////////////

namespace Virtual {

struct BasicThing {
	virtual auto id() const -> size_t = 0;
	virtual ~BasicThing() = default;
};

class SomeThing final : public BasicThing {
	double d;
public:
	SomeThing(double d) : d{d} {}
	auto id() const -> size_t override {
		return static_cast<size_t>(2 * d);
	}
};

class OtherThing final : public BasicThing {
	float f;
public:
	OtherThing(float f) : f{f} {}
	auto id() const -> size_t override {
		return static_cast<size_t>(3 * f);
	}
};

// CONSTRUCTION:
// 1.
//		// Order irrellevant
// 		auto tm = ThingManager(
// 			Thing3(...),
// 			Thing1(...),
// 			Thing2(...),
// 			...
// 		);
// 2.
// 		auto tm = ThingManager();
// 		tm.store<Thing3>(/* args for Thing 3 */));
// 		tm.store<Thing1>(/* args for Thing 1 */));
// 		tm.store<Thing2>(...));
// 		...
//
class ThingManager {
	using UniquePtr = std::unique_ptr<BasicThing>;
	using Vector = std::vector<UniquePtr>;

	Vector things;

public:
	ThingManager() = default;

	ThingManager(auto&&... th) {
		(things.push_back(std::make_unique<std::remove_reference_t<decltype(th)>>(th)), ...);
	}

	template<typename Thing, typename... Args>
	auto store(Args&&... args) {
		things.push_back(std::make_unique<Thing>(std::forward<Args>(args)...));
	}

	auto sum() const -> size_t {
		auto sum = 0ul;
		rs::for_each(things, [&sum](const auto& thing) {
			sum += thing->id();
		});
		return sum;
	}
};

}


///////////////////////////////////////////////////////////////
///////////////////////// NON VIRTUAL /////////////////////////
///////////////////////////////////////////////////////////////

namespace Nonvirtual {

template<typename T, typename... Ts>
concept same_as_any = (... or std::same_as<T, Ts>);

template<typename T>
concept Thing = \
	requires(const T t) {
		{ t.id() } -> std::integral;
	};

class SomeThing {
	double d;
public:
	SomeThing(double d) : d{d} {}
	auto id() const -> size_t {
		return static_cast<size_t>(2 * d);
	}
};

class OtherThing {
	float f;
public:
	OtherThing(float f) : f{f} {}
	auto id() const -> size_t {
		return static_cast<size_t>(3 * f);
	}
};

// CONSTRUCTION:
// 1.
//		// Order irrellevant
// 		auto tm = ThingManager<Thing1, Thing2, ...>(
// 			Thing3(...),
// 			Thing1(...),
// 			Thing2(...),
// 			...
// 		);
// 2.
// 		auto tm = ThingManager<Thing1, Thing2, ...>();
// 		tm.store(Thing3(...));
// 		tm.store(Thing1(...));
// 		tm.store(Thing2(...));
// 		...
//
template<Thing... Th>
class ThingManager {
	template<typename T>
	using Vector = std::vector<T>;

	using Tuple = std::tuple<Vector<Th>...>;

	Tuple things;

	auto _store(same_as_any<Th...> auto&& thing) {
		using ThingType = std::remove_reference_t<decltype(thing)>;
		std::get<Vector<ThingType>>(things).push_back(std::move(thing));
	}

public:
	ThingManager() = default;

	ThingManager(same_as_any<Th...> auto&&... ts) {
		(_store(std::move(ts)), ...);
	}

	auto store(same_as_any<Th...> auto&& t) {
		_store(std::move(t));
	}

	auto sum() const -> size_t {
		return std::apply([](const auto&... ts) {
				auto sum = 0ul;
				(
					rs::for_each(ts, [&sum](const auto& thing) {
						sum += thing.id();
					})
					, ...
				);
				return sum;
			},
			things
		);
	}
};

}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

#ifndef MAX_THINGS
#define MAX_THINGS (500'000)
#endif

#ifdef BENCH
#define SOURCE_SELECTED
#include <benchmark/benchmark.h>

static void virtual_sum(benchmark::State& state) {
	using namespace Virtual;

	auto tm = ThingManager();
	for (auto i = 0; i < MAX_THINGS; ++i) {
		tm.store<SomeThing>(i);
		tm.store<OtherThing>(i);
	}

	for (auto _ : state) {
		const auto s = tm.sum();
		benchmark::DoNotOptimize(s);
	}
}

static void non_virtual_sum(benchmark::State& state) {
	using namespace Nonvirtual;

	auto tm = ThingManager<SomeThing, OtherThing>();
	for (auto i = 0; i < MAX_THINGS; ++i) {
		tm.store(SomeThing(i));
		tm.store(OtherThing(static_cast<float>(i)));
	}

	for (auto _ : state) {
		const auto s = tm.sum();
		benchmark::DoNotOptimize(s);
	}
}


BENCHMARK(virtual_sum);
BENCHMARK(non_virtual_sum);

BENCHMARK_MAIN();
#endif

#ifdef NON_VIRTUAL
#define SOURCE_SELECTED
auto main() -> int {
	using namespace Nonvirtual;

	auto tm = ThingManager<SomeThing, OtherThing>();
	for (auto i = 0; i < MAX_THINGS; ++i) {
		tm.store(SomeThing(i));
		tm.store(OtherThing(static_cast<float>(i)));
	}

	std::cerr << tm.sum() << '\n';
}
#endif

#ifdef VIRTUAL
#define SOURCE_SELECTED
auto main() -> int {
	using namespace Virtual;

	auto tm = ThingManager();
	for (auto i = 0; i < MAX_THINGS; ++i) {
		tm.store<SomeThing>(i);
		tm.store<OtherThing>(i);
	}

	std::cerr << tm.sum() << '\n';
}
#endif

#ifndef SOURCE_SELECTED
#error One of BENCH, MAIN_NON_VIRTUAL, MAIN_VIRTUAL is not defined
#endif

