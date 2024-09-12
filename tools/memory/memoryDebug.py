import re

def parse_struct(file_content):
    # Define GLSL types and their sizes in bytes
    type_sizes = {
        'vec4': 16,  # 4 components * 4 bytes (float)
        'float': 4,
        'uint': 4
    }

    # Regular expression to match struct fields
    field_pattern = re.compile(r'\s*(\w+)\s+(\w+);.*')

    # Split the file content by lines
    lines = file_content.split('\n')

    total_size = 0
    alignment = 0

    for line in lines:
        match = field_pattern.match(line)
        if match:
            type_name, var_name = match.groups()
            size = type_sizes.get(type_name, 0)
            total_size += size
            alignment = max(alignment, size)
            print(f"Field: {var_name}, Type: {type_name}, Size: {size} bytes")

    print(f"\nTotal Size: {total_size} bytes")
    print(f"Alignment: {alignment} bytes")

# Example content of a file with the Particle struct
example_content = """
struct Particle {
    vec4 initial_position;    // initial position    x0
    vec4 pposition;            // predicted position  x~
    vec4 position;            // current position    x
    vec4 new_position;        // predicted position  x+1
    vec4 velocity;            // velocity            u
    vec4 new_velocity;        // velocity            u*
    vec4 acceleration;        // acceleration        a
    float mass;                // mass                m   (or pseudo mass for boundary particles)
    float temperature;        // temperature         T
    float new_temperature;    // new temperature     T+1
    float lambda;            // lagrange multiplier lambda
    uint phase;                // phase (liquid, solid...)
    uint newIndex;            // sorted index
    uint binIndex;            // bin index
};
"""

parse_struct(example_content)