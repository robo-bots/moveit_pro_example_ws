#include <my_behavior/my_behavior.hpp>

#include "spdlog/spdlog.h"
#include <cmath>

namespace my_behavior
{
MyBehavior::MyBehavior(
    const std::string& name, const BT::NodeConfiguration& config,
    const std::shared_ptr<moveit_pro::behaviors::BehaviorContext>& shared_resources)
  : moveit_pro::behaviors::AsyncBehaviorBase(name, config, shared_resources)
{
}


BT::PortsList MyBehavior::providedPorts()
{
  return 
  {
    BT::InputPort<double>("velocity_min", 0.5, "The minimum velocity scale factor"),
    BT::InputPort<double>("velocity_max", 1.0, "The maximum velocity scale factor"),
    BT::OutputPort<double>("velocity_scale_factor", "{velocity_scale_factor}", "The sine wave output for velocity scaling")
  };
}

BT::KeyValueVector MyBehavior::metadata()
{
  // TODO: Define your behavior here.
  return { {"description", "Generates a sine wave output for velocity scaling between min and max values"},{"subcategory", "User Created Behaviors"} };
}

tl::expected<bool,std::string> MyBehavior::doWork() 
{
  const auto ports = moveit_pro::behaviors::getRequiredInputs(getInput<double>("velocity_min"),
                                                                 getInput<double>("velocity_max"));
  if (!ports.has_value()) {
    return tl::make_unexpected("Missing inputs: "+ports.error());
  }
  const auto& [velocity_min, velocity_max] = ports.value();

  // Use a static counter to create a phase for the sine wave
  static int call_count = 0;
  call_count++;

  // Create a phase that increments with each call
  // Adjust the divisor to change how fast the sine wave oscillates
  const double phase = (call_count % 100) * 2.0 * M_PI / 100.0;  // Complete cycle every 100 calls

  // Generate sine wave
  const double sine_value = std::sin(phase);

  // Map sine wave from [-1, 1] to [velocity_min, velocity_max]
  const double amplitude = (velocity_max - velocity_min) / 2.0;
  const double offset = (velocity_max + velocity_min) / 2.0;
  const double velocity_scale_factor = offset + amplitude * sine_value;

  setOutput<double>("velocity_scale_factor", velocity_scale_factor);
  shared_resources_->logger->publishInfoMessage("Velocity Scale Factor:", std::to_string(velocity_scale_factor));
  return { true };
}

tl::expected<void, std::string> MyBehavior::doHalt()
{
  // OPTIONAL: Implement additional logic to handle cancellation of the behavior or objective running it.
  return {};
}

}  // namespace my_behavior
