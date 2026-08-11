#include <behaviortree_cpp/bt_factory.h>
#include <moveit_pro_behavior_interface/behavior_context.hpp>
#include <moveit_pro_behavior_interface/shared_resources_node_loader.hpp>

#include <my_behavior/my_behavior.hpp>

#include <pluginlib/class_list_macros.hpp>

namespace my_behavior
{
class MyBehaviorBehaviorsLoader : public moveit_pro::behaviors::SharedResourcesNodeLoaderBase
{
public:
  void registerBehaviors(BT::BehaviorTreeFactory& factory,
    [[maybe_unused]] const std::shared_ptr<moveit_pro::behaviors::BehaviorContext>& shared_resources) override
  {
    moveit_pro::behaviors::registerBehavior<MyBehavior>(factory, "MyBehavior", shared_resources);
    
  }
};
}  // namespace my_behavior

PLUGINLIB_EXPORT_CLASS(my_behavior::MyBehaviorBehaviorsLoader,
                       moveit_pro::behaviors::SharedResourcesNodeLoaderBase);
