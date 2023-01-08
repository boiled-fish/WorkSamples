// Create Date:    21:08:50 12/07/2020
// Design Name:  boiled_fish
// Module Name:    VGA 
// Project Name: Final project 
// Target Devices: VGA,Sound Sensor
// Description:  use voice to reflect a bouncing ball  
module timecnt(
input clk,
input start,
input rst,
input gameover,
output reg timeover,
output reg [15:0] Time
);
integer counter = 0;
reg out = 0;
always@(posedge clk or posedge rst or posedge start or posedge gameover) begin
    if (rst || !start || gameover) begin
        counter <= 0;
        out <= 0;
    end
    else begin
    if (counter >= 50000000) begin
        counter <= 0;
        out <= ~out;
    end
    else begin
        counter <= counter + 1'b1;
        out <= out;
    end
end
end
always@(posedge out or posedge rst or posedge start or posedge gameover) begin
    if (rst || !start || gameover)begin
        Time <= 10'd30;
        timeover <= 0;
    end
    else begin
        if(Time > 0)begin
        Time <= Time - 1'b1;
        timeover <= 0;
        end
        else begin
        Time <= 0;
        timeover <= 1;
        end
    end
end
endmodule

module display7(
    input clk, rst,
    input [15:0] score,Time,
    
    output reg [6:0] seg_led,
    output reg [7:0] id_led
);
//数码管显示当前得分模块所需变量定义
localparam CLK_DIV = 4'd10;         //时钟分频系数
localparam update = 13'd5000;      //5Mhz分频

reg[3:0]    clk_cnt;
reg         seg_clk;
reg[31:0]   seg_num;
reg[12:0]   cnt;
reg         flag;
reg[2:0]    cnt_id;
reg[3:0]    status;

//每位数码管需要显示的数据
wire[3:0] data1;
wire[3:0] data2;
wire[3:0] data3;
wire[3:0] data4;
wire[3:0] data5;
wire[3:0] data6;
wire[3:0] data7;
wire[3:0] data8;

//分配个数位对应的十进制数据
assign data1 = score % 4'd10;
assign data2 = score / 4'd10 % 4'd10;
assign data3 = score / 7'd100 % 4'd10;
assign data4 = score / 10'd1000 % 4'd10;
//倒计时
assign data5 = Time % 4'd10;
assign data6 = Time / 4'd10 % 4'd10;
assign data7 = Time / 7'd100 % 4'd10;
assign data8 = Time / 10'd1000 % 4'd10;
/////////////////////////////////////////////////////分数显示
//数码管分频5MHz
always@(posedge clk)begin
    if(clk_cnt < CLK_DIV/2 - 1)begin
        clk_cnt <= clk_cnt + 1'b1;
        seg_clk <= seg_clk;
    end
    else begin
        seg_clk <= ~seg_clk;
        clk_cnt <= 4'd0;
    end
end
//将32位的数码管数据转换成8421码
always@(posedge seg_clk)begin
        seg_num[31:28] <= data8;
        seg_num[27:24] <= data7;
        seg_num[23:20] <= data6;
        seg_num[19:16] <= data5;
        seg_num[15:12] <= data4;
        seg_num[11:8]  <= data3;
        seg_num[7:4]   <= data2;
        seg_num[3:0]   <= data1;
end
//定义一个1ms的定时器
always@(posedge seg_clk)begin
    if(cnt < update - 1)begin
        cnt <= cnt + 1'b1;
        flag <= 1'b0;
    end
    else begin
        flag = 1'b1;
        cnt <= 13'd0;
    end
end
//cnt_cs片选计数器，根据该计数器的值确定当前要选那个数码管
always@(posedge seg_clk)begin
    if(flag)begin
        if(cnt_id <4'd8)
            cnt_id <= cnt_id + 1'b1;
        else
            cnt_id <= 3'd0;
    end
    else
        cnt_id <= cnt_id;
end
//根据片选计数器cnt_cs的值，轮流显示每个数码管
always@(posedge seg_clk)begin
    case(cnt_id)
        4'd0:begin
            id_led <= 8'b1111_1110;
            status <= seg_num[3:0];
        end
        4'd1:begin
            id_led <= 8'b1111_1101;
            status <= seg_num[7:4];
        end
        4'd2:begin
            id_led <= 8'b1111_1011;
            status <= seg_num[11:8];
        end
        4'd3:begin
            id_led <= 8'b1111_0111;
            status <= seg_num[15:12];
        end
        4'd4:begin
            id_led <= 8'b1110_1111;
            status <= seg_num[19:16];
        end
        4'd5:begin
            id_led <= 8'b1101_1111;
            status <= seg_num[23:20];
        end
        4'd6:begin
            id_led <= 8'b1011_1111;
            status <= seg_num[27:24];
        end
        4'd7:begin
            id_led <= 8'b0111_1111;
            status <= seg_num[31:28];
        end
        default:begin
            id_led <= 8'b1111_1111;
            status <= 4'd10;
        end                                                        
    endcase
end
//数字的具体显示
always@(*)
begin
    case(status)
    4'b0000:seg_led=7'b1000000;//0
    4'b0001:seg_led=7'b1111001;//1
    4'b0010:seg_led=7'b0100100;//2
    4'b0011:seg_led=7'b0110000;//3
    4'b0100:seg_led=7'b0011001;//4
    4'b0101:seg_led=7'b0010010;//5
    4'b0110:seg_led=7'b0000010;//6
    4'b0111:seg_led=7'b1111000;//7
    4'b1000:seg_led=7'b0000000;//8
    4'b1001:seg_led=7'b0010000;//9
    default:seg_led=7'b1111111;//无显示，全关闭
    endcase
end

endmodule
module vga(
    input clk,rst,start,Dsound,
    input [2:0] hardLevel,
    input [11:0] cir_color,
    input time_over,
    
    output reg gameover,
    output reg [15:0] score,
    output reg hsync,vsync,
    output reg [11:0] color
);
//VGA显示变量定义
reg [9:0] hgrid = 0; // 800 TS , x
reg [9:0] vgrid = 0; // 521 Ts , y
reg clk_4fp = 0;
reg [1:0] count = 0;
//游戏难度
integer speed = 2;
// 显示移动小球
parameter  WIDTH = 24,   //矩形长
            HEIGHT = 24,  //矩形宽
            // 显示区域的边界
            DISV_TOP   = 10'd480,  // display top bound
            DISV_DOWN  = 10'd0,    // display down bound
            DISH_LEFT  = 10'd0,    // display left bound
            DISH_RIGHT = 10'd640;  // display right bound
//
reg [1:0] flag = 0;
reg [9:0] rec_count = 0;
reg [9:0] rec_top_1 = 10'd150;
reg [9:0] rec_down_1 = 10'd100;
reg [9:0] rec_left_1 = 0;
reg [9:0] rec_right_1 = 200;
reg [9:0] rec_top_2 = 10'd350;
reg [9:0] rec_down_2 = 10'd300;
reg [9:0] rec_left_2 = DISH_RIGHT - 10'd200;
reg [9:0] rec_right_2 = DISH_RIGHT;
reg [2:0] loc = 0;
//初始小球的位置，在显示区的左下角
wire [9:0] center_v;
wire [9:0] center_h;
reg [9:0] topbound = DISV_DOWN + HEIGHT;
reg [9:0] downbound = DISV_DOWN;
reg [9:0] leftbound = DISH_LEFT;
reg [9:0] rightbound = DISH_LEFT + WIDTH;
assign center_v = (topbound + downbound)/2;
assign center_h = (leftbound + rightbound)/2;
//初始方向为东南方向
reg [1:0] movexy = 2'b11;
/////////////////////////////////////////////////////VGA显示驱动
//分频 25M Hz
always@(posedge clk)begin
        if(rst)begin
            count<=0;
        end
        else begin
            if(count < 1)begin
            count <= count + 1;
            end
        else
        begin
            clk_4fp <=~clk_4fp;
            count<= 0;
        end
    end
end
// 扫描整个屏幕，包括非显示区,确定什么时候
always @ (posedge clk_4fp or posedge rst) begin
  if(rst) begin
    hgrid <= 0;
    vgrid <= 0;
  end
  else begin
  //根据basic VGA controller的电路图，在水平方向扫描一次后，使得垂直方向开始扫描
  // 因为水平方向是时钟计数的，垂直方向是根据水平方向的脉冲计数的
    if(hgrid >= 800) begin
        hgrid <= 0;
        vgrid <= (vgrid >= 521? 0 : vgrid + 1'b1);
    end
    else
        hgrid <= hgrid + 1'b1;
  end
end
//设置行选，列选信号有效。 由于有建立的Tpw时间，所以要把Tpw(脉冲宽度）时间段内的坐标视为无效
always @(posedge clk_4fp or posedge rst) begin
if(rst) begin
    hsync <= 0;
    vsync <= 0;
end
else begin
    if(hgrid < 752 &&hgrid  >= 656)// 脉冲内为0 (800 - Tbp -Tpw) ~ (800 - Tbp)
        hsync <= 0;
    else
        hsync <= 1;
       
    if(vgrid < 492 && vgrid >= 490 )// 脉冲内为0 (521 - Tbp -Tpw) ~ (521 - Tbp)
        vsync <= 0;
    else 
        vsync <= 1;
end
end
/*
根据时间选择不同范围坐标的像素显示颜色，使其成为一个移动的矩形。
由于是60/s, vsync的Ts恰好是移动1px所花的时间，所以用vsync信号的上升沿判断
*/
//游戏难度选择
always@(hardLevel)begin
case(hardLevel)
3'b001:speed = 2;
3'b011:speed = 4;
3'b111:speed = 6;
default:speed = 1;
endcase
end
//游戏结束条件检测
//确立每一个像素时钟里球的坐标范围
always @ (posedge vsync or posedge rst or posedge start or posedge gameover) begin
if(rst || !start) begin
    topbound   = DISV_DOWN + HEIGHT;
    downbound  = DISV_DOWN;
    leftbound  = DISH_LEFT;
    rightbound = DISH_LEFT + WIDTH;
    movexy = 2'b11;
    score = 0;
    gameover = 0;
end
else if(gameover || time_over) begin
    topbound   = DISV_DOWN + HEIGHT;
    downbound  = DISV_DOWN;
    leftbound  = DISH_LEFT;
    rightbound = DISH_LEFT + WIDTH;
    movexy = 2'b11;
end
//碰到边界，改变方向
else begin
    case(movexy[1:0])
    2'b11: begin // 东南
           if (topbound <= DISV_TOP && topbound >= DISV_TOP - 16 && rightbound < DISH_RIGHT)begin
                if(Dsound > 0)begin
                    movexy = 2'b10;
                    score = score + 1;
                end
                else if(!(Dsound > 0) && topbound <= DISV_TOP && topbound >= DISV_TOP - 16)
                    gameover = 1;
           end
           else if((topbound <= rec_down_1 && topbound >= rec_down_1 - 8 && rightbound > rec_left_1 && rightbound < rec_right_1)||(topbound <= rec_down_2 && topbound >= rec_down_2 - 8 && rightbound < rec_right_2 && rightbound > rec_left_2 ))
                movexy = 2'b10;
           else if ((topbound < DISV_TOP && rightbound <= DISH_RIGHT && rightbound >= DISH_RIGHT - 8)||(topbound < rec_top_1 && topbound > rec_down_1 && rightbound <= rec_left_1 && rightbound >= rec_left_1 - 8)||(topbound < rec_top_2 && topbound > rec_down_2 && rightbound <= rec_right_2  && rightbound >= rec_left_2 - 8))
                movexy = 2'b01;
           else if((topbound <= rec_down_1 && topbound >= rec_down_1 - 8 && rightbound <= rec_left_1 && rightbound >= rec_left_1 - 8)||(topbound <= rec_down_2 && topbound >= rec_down_2 - 8 && rightbound <= rec_left_2 && rightbound >= rec_left_2 - 8))
                movexy = 2'b00;
           else if (topbound <= DISV_TOP && topbound >= DISV_TOP - 16 && rightbound <= DISH_RIGHT && rightbound >= DISH_RIGHT - 20)begin
                if(Dsound>0)begin
                    movexy = 2'b00;
                    score = score + 1;
                end
                else if(!(Dsound > 0) && topbound >= DISV_TOP && topbound >= DISV_TOP - 16)
                    gameover = 1;                  
           end
           end
    2'b10: begin // 东北
           if ((downbound  >= DISV_DOWN && downbound <= DISV_DOWN  + 8 && rightbound < DISH_RIGHT - 8) || (downbound >= rec_top_1 && downbound <= rec_top_1 + 8 && rightbound < rec_right_1 && rightbound > rec_left_1) || (downbound >= rec_top_2 && downbound <= rec_top_2 + 8 && rightbound < rec_right_2 && rightbound > rec_left_2))
                movexy = 2'b11;
           else if ((downbound > DISV_DOWN && rightbound <= DISH_RIGHT && rightbound >= DISH_RIGHT - 8)|| (downbound <= rec_top_1 && downbound >= rec_down_1 && rightbound <= rec_left_1 && rightbound >= rec_left_1 - 8) || (downbound <= rec_top_2 && downbound >= rec_down_2 && rightbound <= rec_left_2 && rightbound >= rec_left_2 - 8) )
                movexy = 2'b00;
           else if ((downbound >= DISV_DOWN && downbound <= DISV_DOWN + 8 && rightbound <= DISH_RIGHT && rightbound >= DISH_RIGHT + 8) || (downbound >= rec_top_1 && downbound <= rec_top_1 + 8 && rightbound <= rec_left_1 && rightbound >= rec_left_1 - 8) || (downbound >= rec_top_2 && downbound <= rec_top_2 + 8 && rightbound <= rec_left_2 && rightbound >= rec_left_2 - 8))
                movexy = 2'b01;
           end
    2'b00: begin // 西北
           if ((downbound >= DISV_DOWN && downbound <= DISV_DOWN + 8 && leftbound > DISH_LEFT) || (downbound >= rec_top_1 && downbound <= rec_top_1 + 8 && rightbound < rec_right_1 && rightbound > rec_left_1)|| (downbound >= rec_top_2 && downbound <= rec_top_2 + 8 && rightbound < rec_right_2 && rightbound > rec_left_2))
                movexy = 2'b01;
           else if ((downbound > DISV_DOWN  && leftbound >= DISH_LEFT && leftbound <= DISH_LEFT + 8)||(downbound < rec_top_1 && downbound > rec_down_1 && leftbound >= rec_right_1 && leftbound <= rec_right_1 + 8)||(downbound < rec_top_2 && downbound > rec_down_2 && leftbound >= rec_right_2 && leftbound <= rec_right_2 + 8))
                movexy = 2'b10;
           else if ((downbound >= DISV_DOWN && downbound <= DISV_DOWN + 8 && leftbound >= DISH_LEFT && leftbound <= DISH_LEFT + 8)|| (downbound >= rec_top_1 && downbound <= rec_top_1 + 8 && leftbound >= rec_right_1 - 4 && leftbound <= rec_right_1 + 8) || (downbound >= rec_top_2 && downbound <= rec_top_2 + 8 && leftbound >= rec_right_2 && leftbound <= rec_right_2 + 8))
                movexy = 2'b11;
           end
    2'b01: begin // 西南
          if (topbound <= DISV_TOP && topbound >= DISV_TOP - 16 && leftbound > DISH_LEFT)begin
                if(Dsound>0)begin
                    movexy = 2'b00;
                    score = score + 1;
                end
                else if(!(Dsound > 0) && topbound >= DISV_TOP && topbound >= DISV_TOP - 16)
                    gameover = 1;                              
           end
           else if((topbound <= rec_down_1 && topbound >= rec_down_1 - 8 && leftbound > rec_left_1 && leftbound < rec_right_1)|| (topbound <= rec_down_2 && topbound >= rec_down_2 - 8 && leftbound > rec_left_2 && leftbound < rec_right_2))begin
                movexy = 2'b00;
           end                
           else if ((topbound < DISV_TOP && leftbound >= DISH_LEFT && leftbound <= DISH_LEFT + 8) || (topbound < rec_top_1 && topbound > rec_down_1 && leftbound >= rec_right_1 && leftbound <= rec_right_1 + 8) || (topbound < rec_top_2 && topbound > rec_down_2 && leftbound >= rec_right_2 && leftbound <= rec_right_2 + 8))
                movexy = 2'b11;
           else if((topbound <= rec_down_1 && topbound >= rec_down_1 - 8 && leftbound >= rec_right_1 && leftbound <= rec_right_1 + 8) || (topbound <= rec_down_2 && topbound >= rec_down_2 - 8 && leftbound >= rec_right_2 && leftbound <= rec_right_2 + 8))
                movexy = 2'b10;
           else if (topbound <= DISV_TOP && topbound >= DISV_TOP - 16 && leftbound >= DISH_LEFT && leftbound <= DISH_LEFT + 20)begin
                if(Dsound>0)begin
                    movexy = 2'b10;
                    score = score + 1;
                end
                else if(!(Dsound > 0) && topbound >= DISV_TOP && topbound >= DISV_TOP - 16)
                    gameover = 1;                               
           end                
           end
    default: movexy = 2'b11;
    endcase
    //操纵方块移动
    topbound   <= topbound   + ( movexy[0]? speed : -speed );
    downbound  <= downbound  + ( movexy[0]? speed : -speed );
    leftbound  <= leftbound  + ( movexy[1]? speed : -speed );
    rightbound <= rightbound + ( movexy[1]? speed : -speed );
    end
end
//生成边框的位置
always @ (posedge vsync or posedge rst or posedge start) begin
    if(rst || !start) begin
        rec_count   <= 0;
        rec_left_1  <= 0;
        rec_right_1 <= 0;
        rec_left_2  <= DISH_RIGHT;
        rec_right_2 <= DISH_RIGHT + 10'd200;
    end
    else if(rec_count >= DISH_RIGHT + 10'd200)begin
        rec_count   <= 0;
        rec_left_1  <= 0;
        rec_right_1 <= 0;
        rec_left_2  <= DISH_RIGHT;
        rec_right_2 <= DISH_RIGHT + 200;    
    end
    else begin
        rec_count <= rec_count + 1'b1;
        rec_right_1 <= rec_count;
        if(rec_count < DISH_RIGHT)
            rec_left_2 <= DISH_RIGHT - rec_count;
        else if(rec_count >= DISH_RIGHT)
            rec_left_2 <= 0;
        if(rec_count >= 10'd200)begin
            rec_left_1 <= rec_count - 10'd200;
            rec_right_2 <= DISH_RIGHT + 10'd200 - rec_count;
        end
    end
end

// 确定扫描到哪一个像素该显示什么颜色
always @(posedge clk_4fp or posedge rst) begin
if(rst)begin
     color <= 12'b0000_0000_0000;
end
else if(!start)begin
    if((hgrid>= 124 && hgrid <= 144) && (vgrid >= 150 && vgrid <=300))
        color <= 12'b1111_1111_1111;
    else if((hgrid >= 124 && hgrid <=244) && (vgrid >= 150 && vgrid <=170))
        color <= 12'b1111_1111_1111;
    else if((hgrid >= 124 && hgrid <= 244) && (vgrid >= 280 && vgrid <=300))
        color <= 12'b1111_1111_1111;
    else if((hgrid >= 184 && hgrid <= 244) && (vgrid >= 225 && vgrid <=245))
        color <= 12'b1111_1111_1111;
    else if((hgrid >= 224 && hgrid <= 244) && (vgrid >= 225 && vgrid <= 300))
        color <= 12'b1111_1111_1111;
    else if((hgrid >= 304 && hgrid <= 424) && (vgrid >= 150 && vgrid <= 170))
        color <= 12'b1111_1111_1111;
    else if((hgrid >= 304 && hgrid <= 424) && (vgrid >= 280 && vgrid <= 300))
        color <= 12'b1111_1111_1111;
    else if((hgrid >= 304 && hgrid <= 324) && (vgrid >= 150 && vgrid <= 300))
        color <= 12'b1111_1111_1111;
    else if((hgrid >= 404 && hgrid <= 424) && (vgrid >= 150 && vgrid <= 300))
        color <= 12'b1111_1111_1111;
    else if((hgrid >= 484 && hgrid <= 504) && (vgrid >= 150 && vgrid<= 260))
        color <= 12'b1111_1111_1111;
    else if((hgrid >= 484 && hgrid<= 504) && (vgrid >= 280 && vgrid <= 290))
        color <= 12'b1111_1111_1111;
    else 
        color <= 12'b0;
end
else if(gameover || time_over)begin
    if((hgrid>= 124 && hgrid <= 144) && (vgrid >= 150 && vgrid <=300))
        color <= 12'b0000_1111_0000;
    else if((hgrid >= 124 && hgrid <=244) && (vgrid >= 150 && vgrid <=170))
        color <= 12'b0000_1111_0000;
    else if((hgrid >= 124 && hgrid <= 244) && (vgrid >= 280 && vgrid <=300))
        color <= 12'b0000_1111_0000;
    else if((hgrid >= 184 && hgrid <= 244) && (vgrid >= 225 && vgrid <=245))
        color <= 12'b0000_1111_0000;
    else if((hgrid >= 224 && hgrid <= 244) && (vgrid >= 225 && vgrid <= 300))
        color <= 12'b0000_1111_0000;
    else if((hgrid>= 304 && hgrid <= 324) && (vgrid >= 150 && vgrid <=300))
        color <= 12'b0000_1111_0000;
    else if((hgrid >= 304 && hgrid <= 424) && (vgrid >= 150 && vgrid <=170))
        color <= 12'b0000_1111_0000;
    else if((hgrid >= 304 && hgrid <= 424) && (vgrid >= 280 && vgrid <=300))
        color <= 12'b0000_1111_0000;
    else if((hgrid >= 364 && hgrid <= 424) && (vgrid >= 225 && vgrid <=245))
        color <= 12'b0000_1111_0000;
    else if((hgrid >= 404 && hgrid <= 424) && (vgrid >= 225 && vgrid <= 300))
        color <= 12'b0000_1111_0000;
    else if((hgrid >= 484 && hgrid <= 504) && (vgrid >= 150 && vgrid<= 260))
        color <= 12'b0000_1111_0000;
    else if((hgrid >= 484 && hgrid<= 504) && (vgrid >= 280 && vgrid <= 300))
        color <= 12'b0000_1111_0000;
    else 
        color <= 12'b0;
end
else begin
    if (hgrid >= DISH_LEFT  && hgrid <= DISH_RIGHT  && vgrid >= DISV_DOWN && vgrid <= DISV_TOP) begin
        if((hgrid-center_h)*(hgrid - center_h)+(vgrid - center_v)*(vgrid - center_v) <= 144)
            color <= cir_color;//小球颜色
        else if((hgrid <= rec_right_1 && hgrid >= rec_left_1 && vgrid >= rec_down_1 && vgrid <= rec_top_1) || (hgrid >= rec_left_2 && hgrid <= rec_right_2 && vgrid >= rec_down_2 && vgrid <= rec_top_2))
                color <= 12'b1111_1111_1111;                    
        else
            color <= 12'b0001_0000_0000; //
    end
    else begin
       color <= 12'b0000_0000_0000;
    end
    end
end
endmodule


module Ball_game(
    input clk,              //系统时钟
    input rst,              //复位键
    input Dsound,           //声音传感器数字信号
    input Asound,           //声音传感器模拟信号
    input start,            //游戏开关
    input [2:0] speed,      //小球速度
    input [11:0] cir_color, //小球颜色

    output [6:0] seg_led,   //七段数码管显示数据
    output [7:0] id_led,    //七段数码管选择
    output hsync,vsync,     //行有效&列有效信号
    output [11:0] color     //VGA颜色输出
);
wire time_over,gameover;
wire [15:0] score,Time;
vga show_vga(clk,rst,start,Dsound,speed,cir_color,time_over,gameover,score,hsync,vsync,color);
timecnt(clk,start,rst,gameover,time_over,Time);
display7 display_score(clk,rst,score,Time,seg_led,id_led);


endmodule