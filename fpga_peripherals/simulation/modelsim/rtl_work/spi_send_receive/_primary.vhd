library verilog;
use verilog.vl_types.all;
entity spi_send_receive is
    port(
        state           : in     vl_logic;
        reset           : in     vl_logic;
        sck             : in     vl_logic;
        sdi             : in     vl_logic;
        sdo             : out    vl_logic;
        d               : in     vl_logic_vector(7 downto 0);
        q               : out    vl_logic_vector(7 downto 0);
        data_exchanged  : out    vl_logic
    );
end spi_send_receive;
