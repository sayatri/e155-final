library verilog;
use verilog.vl_types.all;
entity display_bargraph is
    port(
        clk             : in     vl_logic;
        reset           : in     vl_logic;
        ready_to_listen : in     vl_logic;
        listening       : in     vl_logic;
        processing      : in     vl_logic;
        done            : in     vl_logic;
        bar_R           : out    vl_logic_vector(3 downto 0);
        bar_G           : out    vl_logic_vector(3 downto 0)
    );
end display_bargraph;
