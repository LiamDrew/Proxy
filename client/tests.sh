# #!/bin/bash

# # NOTE: 
# # Assumes the proxy is running on port 9052
# # Assumes the cache size of the proxy is 2

# # NOTE: Cannot be used with test2, since they both fetch from the same website
# # at the same port
# # Test 1.5: Fetch a larger file
# # curl -x 10.4.2.20:9052 http://www.cs.cmu.edu/~prs/bio.html > t2_output
# # curl http://www.cs.cmu.edu/~prs/bio.html > t2

# # diff t2_output t2

# # if [ $? -eq 0 ]; then
# #     echo "Test 2 Passed"
# # else
# #     echo "Test 2 FAILED"
# # fi
# # echo -e "______________________________________\n"

# # rm "t2_output"
# # rm "t2"

# # # Test 1: Fetch a simple file
curl -x [IP_ADDRESS] http://www.cs.tufts.edu/comp/112/index.html > t1_output
curl http://www.cs.tufts.edu/comp/112/index.html > t1

diff t1_output t1
if [ $? -eq 0 ]; then
    echo "Test 1 Passed"
else
    echo "Test 1 FAILED"
fi
echo -e "______________________________________\n"

rm "t1_output"
rm "t1"


# # # Test 2: Fetch a binary file
# curl -x 10.4.2.20:9052 http://www.cs.cmu.edu/~dga/dga-headshot.jpg > t2_output.jpg
# curl http://www.cs.cmu.edu/~dga/dga-headshot.jpg > t2.jpg

# diff t2_output.jpg t2.jpg

# if [ $? -eq 0 ]; then
#     echo "Test 2 Passed"
# else
#     echo "Test 2 FAILED"
# fi
# echo -e "______________________________________\n"

# rm "t2.jpg"
# rm "t2_output.jpg"



# # Test 4: Fetch from a port other than 80
# echo "Starting server1 on port 9053..."
# python server1.py --port 9053 &
# SERVER1_PID=$!

# sleep 1

# echo "Making request to server1 through proxy..."
# curl -x 10.4.2.20:9052 http://10.4.2.20:9053 > server1_response
# curl http://10.4.2.20:9053 > response_1

# echo -e "______________________________________\n"
# diff server1_response response_1
# if [ $? -eq 0 ]; then
#     echo "Test 4 Passed"
# else
#     echo "Test 4 FAILED"
# fi
# echo -e "______________________________________\n"

# kill $SERVER1_PID
# rm "server1_response"
# rm "response_1"



# # Test 5: Some host but different port numbers get cache differently
# # Start server1 and server2 in the background
# echo "Starting server1 on port 9053..."
# python server1.py --port 9053 &
# SERVER1_PID=$!

# echo "Starting server2 on port 9054..."
# python server2.py --port 9054 &
# SERVER2_PID=$!

# # Give the servers some time to start
# sleep 1

# curl -x 10.4.2.20:9052 http://10.4.2.20:9053 > server1_response
# curl http://10.4.2.20:9053 > response_1

# curl -x 10.4.2.20:9052 http://10.4.2.20:9054 > server2_response
# curl http://10.4.2.20:9054 > response_2

# diff server1_response response_1
# result1=$?

# diff server2_response response_2
# result2=$?

# echo -e "\n______________________________________"
# if [ $result1 -eq 0 ] && [ $result2 -eq 0 ]; then
#     echo "Test 5 Passed"
# else
#     echo "Test 5 FAILED"
# fi
# echo -e "______________________________________\n"


# # Kill the server processes after the test
# kill $SERVER1_PID
# kill $SERVER2_PID
# rm "server1_response"
# rm "response_1"
# rm "server2_response"
# rm "response_2"


# Test 6: Make sure the cache can server fresh content without pinging the server
# echo "Starting server1 on port 9053..."
# python server_2ping.py --port 9053 &
# SERVER1_PID=$!

# sleep 1

# echo "Making request to server1 through proxy..."
# curl http://10.4.2.20:9053 > response_1
# curl -x 10.4.2.20:9052 http://10.4.2.20:9053 > server1_response
# curl -x 10.4.2.20:9052 http://10.4.2.20:9053 > server1_response
# curl -x 10.4.2.20:9052 http://10.4.2.20:9053 > server1_response


# echo -e "______________________________________\n"
# diff server1_response response_1
# if [ $? -eq 0 ]; then
#     echo "Test 6 Passed"
# else
#     echo "Test 6 FAILED"
# fi
# echo -e "______________________________________\n"

# kill $SERVER1_PID
# rm "response_1"
# rm "server1_response"


# # Test 7: Proxy will refetch a stale item
# echo "Starting server1 on port 9053..."
# python server_stale.py --port 9053 &
# SERVER1_PID=$!

# sleep 1

# curl -x 10.4.2.20:9052 http://10.4.2.20:9053 > proxy_first_output
# curl http://10.4.2.20:9053 > first_output


# sleep 2

# curl http://10.4.2.20:9053 > second_output
# curl -x 10.4.2.20:9052 http://10.4.2.20:9053 > proxy_second_output

# diff first_output proxy_first_output
# result1=$?

# diff second_output proxy_second_output
# result2=$?

# echo -e "\n______________________________________"
# if [ $result1 -eq 0 ] && [ $result2 -eq 0 ]; then
#     echo "Test 7 Passed"
# else
#     echo "Test 7 FAILED"
# fi
# echo -e "______________________________________\n"

# kill $SERVER1_PID
# rm "first_output"
# rm "proxy_first_output"
# rm "second_output"
# rm "proxy_second_output"

# Test 8: Make sure Age: does not appear on a fresh fetch, but does appear for
# a cached fetched

# This is working, but the test is fucked

# # Perform the first curl request
# curl -i -x 10.4.2.20:9052 http://www.cs.tufts.edu/comp/112/index.html > t1_output

# # Wait for 2 seconds
# sleep 2

# # Perform the second curl request
# curl -i -x 10.4.2.20:9052 http://www.cs.tufts.edu/comp/112/index.html > t2_output

# # Extract the Age field from both outputs
# age1=$(grep -oP 'Age: \K[0-9]+' "t1_output")
# age2=$(grep -oP 'Age: \K[0-9]+' "t2_output")

# echo "$age1"
# echo "$age2"



# # Check if the age1 variable is empty
# if [ -z "$age1" ]; then
#     echo "Test 8 Part 1 Passed"
# else
#     echo "Test 8 Part 1 FAILED"
# fi

# # Check if the age2 variable is empty
# if [ -z "$age2" ]; then
#     echo "Test 8 Part 2 FAILED"
# else
#     # Check if age2 is equal to 2
#     if [ "$age2" -eq 2 ]; then
#         echo "Test 8 Part 2 Passed"
#     else 
#         echo "Test 8 Part 2 FAILED"
#     fi
# fi


# curl -i -x 10.4.2.20:9052 http://www.cs.tufts.edu/comp/112/index.html > t1_output

# sleep 2

# curl -i -x 10.4.2.20:9052 http://www.cs.tufts.edu/comp/112/index.html > t2_output

# age1=$(grep -oP 'Age: \K[0-9]+' "t1_output")
# age2=$(grep -oP 'Age: \K[0-9]+' "t2_output")


# # # Check if the age variable is empty
# # if [ -z "$age1" ]; then
# #     echo "Test 8 Part 1 passed"
# # else
# #     echo "Test 8 Part 1 FAILED"
# # fi

# # # Check if the age variable is empty
# # if [ -z "$age2" ]; then
# #     echo "Test 8 Part 2 FAILED"
# # else
# #     if [ "$age2" -eq 2 ]; then
# #         echo "Test 8 Part 2 Passed"


# #     else 
# #         echo "Test 8 Part 2 FAILED"

# #     fi
# # fi

# Test 9: Purge least recent item from cache

# python server1.py --port 9053 &
# SERVER1_PID=$!

# sleep 1

# curl -x 10.4.2.20:9052 http://www.cs.tufts.edu/comp/112/index.html > first

# curl -x 10.4.2.20:9052 http://www.cs.cmu.edu/~prs/bio.html > cmu

# curl -x 10.4.2.20:9052 http://www.cs.tufts.edu/comp/112/index.html > second

# rm "first"
# rm "cmu"
# rm "second"
# rm "example"






# Test 10: Purge stale item from cache:
# # Start server1 and server2 in the background
# echo "Starting server1 on port 9053..."
# python server1.py --port 9053 &
# SERVER1_PID=$!

# server 2
# echo "Starting server2 on port 9054..."
# python server2.py --port 9054 &
# SERVER2_PID=$!

# # Give the servers some time to start
# sleep 1

# # Fetch content from server 1 (Max Age 20 seconds)
# curl -x 10.4.2.20:9052 http://10.4.2.20:9053 > server1_response

# # Fetch content from server 2 (Max Age 1 second)
# curl -x 10.4.2.20:9052 http://10.4.2.20:9054 > server2_response

# sleep 3

# # should evict 1 item from cache
# curl -x 10.4.2.20:9052 http://www.cs.cmu.edu/~prs/bio.html > cmu

# sleep 1

# # should be served quickly, in the cache
# curl -x 10.4.2.20:9052 http://10.4.2.20:9053 > server1_response



# # should be served slowly, not in the cache
# curl -x 10.4.2.20:9052 http://10.4.2.20:9054 > server2_response




# # Kill the server processes after the test
# kill $SERVER1_PID
# kill $SERVER2_PID
# rm "server1_response"
# rm "response_1"
# rm "cmu"
# rm "server2_response"

