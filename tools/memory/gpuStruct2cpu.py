
# Example content of a GLSL file with the Particle struct
glsl_content = """
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

def glsl_to_aligned_cpp_struct(glsl_struct_content):
    # GLSL to C++ type mapping with alignment
    type_mapping = {
        'vec4': ('glm::vec4', 16),
        'float': ('float', 4),
        'uint': ('unsigned int', 4)
    }

    # Regular expression to match struct fields
    field_pattern = re.compile(r'\s*(\w+)\s+(\w+);.*')

    # Split the GLSL struct content by lines
    lines = glsl_struct_content.split('\n')

    cpp_struct = "struct Particle {\n"
    for line in lines:
        match = field_pattern.match(line)
        if match:
            glsl_type, var_name = match.groups()
            cpp_type, alignment = type_mapping.get(glsl_type, ("UnknownType", 0))
            align_str = f"alignas({alignment}) " if alignment > 4 else ""
            cpp_struct += f"    {align_str}{cpp_type} {var_name};\n"
    
    # Consider adding explicit padding here if needed

    cpp_struct += "};"

    return cpp_struct

# Use the glsl_to_aligned_cpp_struct function with the GLSL content
cpp_struct = glsl_to_aligned_cpp_struct(glsl_content)
cpp_struct

