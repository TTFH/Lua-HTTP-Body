body = {
    tools = {
        [1] = {
            ["function"] = {
                name = "drive_vehicle",
                description = "Drives a vehicle, it may return a status report, for example if the vehicle reached it's destination.",
                parameters = {
                    type = "object",
                    required = {[1] = "vehicle", [2] = "destination"},
                    properties = {
                        destination = {
                            description = "The target location in the format Vec(x, y, z), special vehicles will perform an action when reaching it's target.",
                            type = "string"
                        },
                        vehicle = {
                            description = "The vehicle handle, if it's not the current vehicle the user will disembark and walk to it.",
                            type = "integer"
                        }
                    }
                }
            },
            type = "function"
        },
        [2] = {
            ["function"] = {
                name = "move_crane_arm",
                description = "If driving a crane it extends or contracts it's arm, crane can be hooked or not."
            },
            type = "function"
        }
    },
    messages = {
        [1] = {content = "You're a smart NPC in a game, you can take actions on your own.", role = "system"},
        [2] = {
            content = "Your objective is to drive that van 'handle = 12' to the garage 'pos = Vec(5, 0, -17.9)', there is a crane 'handle = 23' nearby that you can use if you get stuck.",
            role = "user"
        },
        [3] = {
            tool_calls = {
                [1] = {
                    id = "call_1",
                    type = "function",
                    ["function"] = {
                        name = "drive_vehicle",
                        arguments = '{"vehicle":12,"destination":"Vec(5, 0, -17.9)"}'
                    }
                }
            },
            role = "assistant"
        },
        [4] = {
            content = "walking to van, entering van, driving van to garage, the van got stuck at location Vec(-6, 0, 12)",
            role = "tool",
            name = "drive_vehicle",
            tool_call_id = "call_1"
        },
        [5] = {
            tool_calls = {
                [1] = {
                    id = "call_2",
                    type = "function",
                    ["function"] = {name = "drive_vehicle", arguments = '{"vehicle":23,"destination":"Vec(-6, 0, 12)"}'}
                }
            },
            role = "assistant"
        },
        [6] = {
            content = "disembarking van, walking to crane, entering crane, driving crane to van, crane has reached it's destination, crane hook connected",
            role = "tool",
            name = "drive_vehicle",
            tool_call_id = "call_2"
        },
        [7] = {
            tool_calls = {
                [1] = {id = "call_3", type = "function", ["function"] = {name = "move_crane_arm", arguments = "{}"}}
            },
            role = "assistant"
        },
        [8] = {
            content = "crane moved the attached object",
            role = "tool",
            name = "move_crane_arm",
            tool_call_id = "call_3"
        },
        [9] = {
            tool_calls = {
                [1] = {
                    id = "call_4",
                    type = "function",
                    ["function"] = {
                        name = "drive_vehicle",
                        arguments = '{"vehicle":12,"destination":"Vec(5, 0, -17.9)"}'
                    }
                }
            },
            role = "assistant"
        },
        [10] = {
            content = "crane hook disconnected, disembarking crane, walking to van, entering van, driving van to garage, van has reached it's destination",
            role = "tool",
            name = "drive_vehicle",
            tool_call_id = "call_4"
        },
        [11] = {
            content = "The van has been successfully driven to the garage. Mission accomplished!",
            role = "assistant"
        }
    },
    model = "gpt-4o"
}
