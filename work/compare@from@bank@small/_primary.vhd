library verilog;
use verilog.vl_types.all;
entity compareFromBankSmall is
    port(
        state           : in     vl_logic_vector(2 downto 0);
        clk             : in     vl_logic;
        resultCompare   : out    vl_logic;
        finalIndex      : out    vl_logic;
        transmit_ready  : out    vl_logic
    );
end compareFromBankSmall;
