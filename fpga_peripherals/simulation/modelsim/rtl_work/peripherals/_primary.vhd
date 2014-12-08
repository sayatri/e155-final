library verilog;
use verilog.vl_types.all;
entity peripherals is
    port(
        clk             : in     vl_logic;
        sck             : in     vl_logic;
        sdi             : in     vl_logic;
        ss              : in     vl_logic;
        reset           : in     vl_logic;
        sdo             : out    vl_logic;
        bar_R           : out    vl_logic_vector(3 downto 0);
        bar_G           : out    vl_logic_vector(3 downto 0);
        led_rdy         : out    vl_logic;
        led_rec         : out    vl_logic;
        led_R           : out    vl_logic;
        led_G           : out    vl_logic;
        led_Y           : out    vl_logic;
        led_O           : out    vl_logic;
        led             : out    vl_logic_vector(3 downto 0)
    );
end peripherals;
