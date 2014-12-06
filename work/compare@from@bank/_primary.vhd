library verilog;
use verilog.vl_types.all;
entity compareFromBank is
    port(
        state           : in     vl_logic_vector(2 downto 0);
        clk             : in     vl_logic;
        resultOne       : out    vl_logic_vector(14 downto 0);
        resultTwo       : out    vl_logic_vector(14 downto 0);
        maxIndexOne     : out    vl_logic_vector(11 downto 0);
        maxIndexTwo     : out    vl_logic_vector(11 downto 0);
        transmit_ready  : out    vl_logic
    );
end compareFromBank;
