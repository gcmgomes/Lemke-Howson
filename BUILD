package(default_visibility = ["//visibility:public"])

cc_library(
  name = "game",
  srcs = ["game.cc"],
  hdrs = ["game.h"],
)

cc_binary(
  name = "game_test",
  srcs = ["game_test.cc"],
  deps = ["//:game"],
)

cc_library(
  name = "linear_equation",
  srcs = ["linear_equation.cc"],
  hdrs = ["linear_equation.h"],
)

cc_binary(
  name = "linear_equation_test",
  srcs = ["linear_equation_test.cc"],
  deps = ["//:linear_equation"],
)

cc_library(
  name = "linear_system",
  srcs = ["linear_system.cc"],
  hdrs = ["linear_system.h"],
  deps = ["//:game",
          "//:linear_equation"],
)

cc_binary(
  name = "linear_system_test",
  srcs = ["linear_system_test.cc"],
  deps = ["//:linear_system"],
)

cc_library(
  name = "heuristic",
  srcs = ["heuristic.cc"],
  hdrs = ["heuristic.h"],
  deps = ["//:linear_system"],
)

cc_binary(
  name = "heuristic_test",
  srcs = ["heuristic_test.cc"],
  deps = ["//:heuristic"],
)

cc_binary(
  name =  "generator",
  srcs = ["generator.cc"],
)
