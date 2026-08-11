import threading
import tkinter as tk

import rclpy
from rclpy.action import ActionClient
from rclpy.node import Node

from moveit_studio_sdk_msgs.action import DoObjectiveSequence


class ObjectiveLauncher(Node):
  def __init__(self):
      super().__init__("start_my_application")
      self._client = ActionClient(self, DoObjectiveSequence, "/do_objective")
      self._goal_handle = None

  def start(self, objective_name):
      if not self._client.wait_for_server(timeout_sec=2.0):
          self.get_logger().error("Action server not available")
          return
      goal = DoObjectiveSequence.Goal(objective_name=objective_name)
      future = self._client.send_goal_async(goal)
      future.add_done_callback(self._on_goal_response)

  def _on_goal_response(self, future):
      self._goal_handle = future.result()
      if not self._goal_handle.accepted:
          self.get_logger().info("Objective rejected")
          return
      self._goal_handle.get_result_async().add_done_callback(self._on_result)

  def _on_result(self, future):
      result = future.result().result
      self.get_logger().info(f"Objective finished: code={result.error_code.val}")
      self._goal_handle = None

  def cancel(self):
      if self._goal_handle is None:
          self.get_logger().info("No active Objective to cancel")
          return
      self._goal_handle.cancel_goal_async()


def main():
  rclpy.init()
  node = ObjectiveLauncher()

  # Spin ROS in a background thread so tkinter stays responsive.
  spin_thread = threading.Thread(target=rclpy.spin, args=(node,), daemon=True)
  spin_thread.start()

  window = tk.Tk()
  window.title("Start My Very Own Application Example")

  frame = tk.Frame(window)
  frame.pack(pady=20)

  tk.Button(
      frame,
      text="Start",
      command=lambda: node.start("My Application"),
      height=2,
      width=20,
  ).pack(side="left", padx=10)

  tk.Button(
      frame,
      text="Stop",
      command=node.cancel,
      height=2,
      width=20,
  ).pack(side="left", padx=10)

  window.mainloop()

  node.destroy_node()
  rclpy.shutdown()


if __name__ == "__main__":
  main()

