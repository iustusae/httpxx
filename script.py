import aiohttp
import asyncio
import time
from statistics import mean
import argparse


async def make_request(session, url, request_num):
    start_time = time.time()
    try:
        async with session.get(url) as response:
            await response.text()
            return time.time() - start_time
    except Exception as e:
        print(f"Request {request_num} failed: {e}")
        return None


async def load_test(num_requests, url):
    async with aiohttp.ClientSession() as session:
        start_time = time.time()

        # Create list of tasks for concurrent execution
        tasks = [make_request(session, url, i) for i in range(num_requests)]

        # Execute all requests concurrently
        request_times = await asyncio.gather(*tasks)

        # Filter out failed requests
        successful_times = [t for t in request_times if t is not None]

        total_time = time.time() - start_time

        # Calculate statistics
        if successful_times:
            avg_request_time = mean(successful_times)
            num_failed = len(request_times) - len(successful_times)

            print(f"\nLoad Test Results:")
            print(f"Total requests: {num_requests}")
            print(f"Successful requests: {len(successful_times)}")
            print(f"Failed requests: {num_failed}")
            print(f"Average request time: {avg_request_time:.4f} seconds")
            print(f"Total test time: {total_time:.4f} seconds")
            print(f"Requests per second: {num_requests/total_time:.2f}")
        else:
            print("All requests failed!")


def main():
    parser = argparse.ArgumentParser(description='HTTP Load Testing Script')
    parser.add_argument('--requests', type=int, default=100,
                        help='number of requests to make (default: 100)')
    parser.add_argument('--url', type=str, default='http://localhost:9999',
                        help='target URL (default: http://localhost:9999)')

    args = parser.parse_args()

    print(f"Starting load test with {args.requests} requests to {args.url}")

    asyncio.run(load_test(args.requests, args.url))


if __name__ == "__main__":
    main()
