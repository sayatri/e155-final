library verilog;
use verilog.vl_types.all;
entity correlate is
    port(
        topState        : in     vl_logic_vector(2 downto 0);
        clk             : in     vl_logic;
        a               : in     vl_logic;
        b               : in     vl_logic;
        finished        : out    vl_logic;
        maxResult       : out    vl_logic_vector(35 downto 0);
        maxIndex        : out    vl_logic_vector(11 downto 0)
    );
end correlate;
