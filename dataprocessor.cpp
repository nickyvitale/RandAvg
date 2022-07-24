#include <mqtt_client_cpp.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <iostream>


boost::asio::io_service ios;
boost::asio::deadline_timer timer(ios);

// Will be called after 3 minutes to terminate
// the run() of the io_service
void expiration_handler(boost::system::error_code ec){
	ios.stop();
}

// Convertes MQTT_NS buffer (string_view) to integer
int bufferToInt(MQTT_NS::buffer val){
	namespace qi = boost::spirit::qi;
	int dest;
	qi::parse(val.begin(), val.end(), qi::int_, dest);
	return dest;
}

// Connects to localhost on port 1883, subscribes to "random"
// Then takes a count of all numbers read in order to 
// calculated average after 3 minutes of reading
int main(){
	timer.expires_from_now(boost::posix_time::seconds(180));
	timer.async_wait(expiration_handler);

	auto c = mqtt::make_client(ios, "localhost", "1883");
	
	// variables used for calculation
	int sum = 0;
	int numsAdded = 0;

	// setup client
	c->set_client_id("cid1");
	c->set_clean_session(true);

	// setup handler functions
	c->set_connack_handler(
		[&c](bool sp, mqtt::connect_return_code rc){
			if (rc == mqtt::connect_return_code::accepted){
				c->subscribe("random", mqtt::qos::at_most_once);
				return true;
			}
			return false;
		}
	);

	c->set_close_handler(
		[]
		(){
			std::cout << "closed\n";
		}
	);

	c->set_error_handler(
		[]
		(mqtt::error_code ec){
			std::cout << "error: " << ec.message() << "\n";
		}
	);

	c->set_publish_handler(
		[&]
		(boost::optional<short unsigned int> packetId,
		 MQTT_NS::publish_options pubOpts,
		 MQTT_NS::buffer topicName,
		 MQTT_NS::buffer contents){
			int latestNum = bufferToInt(contents);
			if (latestNum  >= 0 && latestNum <= 10){ // if valid input
				std::cout << "latest number: " <<
			       	latestNum << "\n";
				numsAdded++;
				sum += bufferToInt(contents);
			}
			return true;
		}
	);
	
	c->connect();
	ios.run();
	std::cout << "Average over three minutes: " <<
		     (float)sum / (float) numsAdded << "\n";


	return 0;
}
