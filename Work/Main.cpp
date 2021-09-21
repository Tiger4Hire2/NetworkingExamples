#include <asio.hpp>

// a class to split the job 
class FloodFill
{
public:
    asio::io_context    context;
    std::vector<float>  mem_block;
public:
    void Fill(float value, int size);
    void Work();
    float GetValue(int i) { return mem_block[i]; }

};

void FloodFill::Fill(float val, int size)
{
    mem_block.resize(size*1024);
    std::ptrdiff_t step = mem_block.size()/1024;
    for(size_t i =0; i < mem_block.size(); i+=step)
        context.post( [this, i, val, step](){ std::fill(mem_block.begin()+i, mem_block.begin()+i+step, val); });
}

void FloodFill::Work()
{
    context.run();
}

#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
static void FillTime(benchmark::State &state)
{
    FloodFill filler;
    std::future<void> thread_result[4];
    for (auto _ : state)
    {
        filler.Fill(1.3f, 1024);
        for (int i = 0; i < state.range(); ++i)
            thread_result[i] = std::async(std::launch::async, [&filler](){filler.Work();});
        for (int i = 0; i < state.range(); ++i)
            thread_result[i].wait();
    }
}

BENCHMARK(FillTime)->DenseRange(1,4);
TEST(FloodFill, Proof)
{
    FloodFill filler;
    filler.Fill(1.3f, 1024);
    std::future<void> thread_result[4];
    for (size_t i = 0; i < std::size(thread_result); ++i)
        thread_result[i] = std::async(std::launch::async, [&filler](){filler.Work();});
    for (size_t i = 0; i < std::size(thread_result); ++i)
        thread_result[i].wait();
    EXPECT_EQ(filler.GetValue(0), 1.3f);
    EXPECT_EQ(filler.GetValue(1023), 1.3f);
    EXPECT_EQ(filler.GetValue(1024), 1.3f);
    EXPECT_EQ(filler.GetValue(1024*(1024-1)), 1.3f);
    EXPECT_EQ(filler.GetValue(1024*1024-1), 1.3f);
}
