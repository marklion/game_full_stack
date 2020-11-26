<template>
<div class="each_player">
    <el-card :body-style="{ padding: '3px' }" class="player_show_class" :style="seat_no_background(seat_no)">
        <div>
            <el-avatar :src="player_info.logo" :size="50"></el-avatar>
        </div>
        <div>
            {{player_info.name}}
        </div>
        <hr>
        <div>
            <i class="el-icon-money">{{player_info.total_cash}}</i>
        </div>
        <div>
            <i class="el-icon-bottom-right">{{player_info.bat_cash}}</i>
        </div>
        <el-dialog title="携带金额" :visible.sync="centerDialogVisible" center>
            <el-row>
                <el-input-number v-model="carry_cash" :step="1000"></el-input-number>
            </el-row>
            <el-row>
                <el-col :span="12">
                    <el-button @click="centerDialogVisible = false">取 消</el-button>
                </el-col>
                <el-col :span="12">
                    <el-button type="primary" @click="sit_down_proc">确 定</el-button>
                </el-col>
            </el-row>
        </el-dialog>
        <el-button @click="centerDialogVisible = true" v-if="need_show_sit_down()">坐下</el-button>
    </el-card>
</div>
</template>

<script>
export default {
    name: 'EachPlayer',
    props: {
        seat_no: Number,
        player_info: {
            name: "",
            logo: "",
            total_cash: 0,
            bat_cash: 0,
        },
        sit_down_proc_in: Function,
        show_sit_down: Boolean,
        dealer_pos: Number,
        action_pos: Number,
    },
    data: function () {
        return {
            centerDialogVisible: false,
            carry_cash: 1000,
            need_show_sit_down: function () {
                if (this.player_info.logo.length == 0 && this.show_sit_down)
                    return true;
                else
                    return false;
            },
            seat_no_background:function(_seat_no) {
                var seat_no_show = _seat_no + 1;
                var img_url = "http://www.d8sis.cn/game_resource/number_background/red_num_0"+seat_no_show+".png";
                var color = 'lightgoldenrodyellow';

                return { 
                    background: "url(" + img_url + ")", 
                    'background-size': 'auto ' + "15%", 
                    'background-repeat': "no-repeat", 
                    'background-color': color
                };           
            },
        }
    },
    methods: {
        sit_down_proc: function () {
            this.centerDialogVisible = false;
            this.sit_down_proc_in(this.seat_no, this.carry_cash);
        },
    },
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->

<style lang="scss" scoped>
.el-button {
    display: block;
    width: 100%;
    padding-left: 4px;
    padding-right: 4px;
}

.el-input-number {
    display: block;
    width: 100%;
    padding-left: 4px;
    padding-right: 4px;
}

.money_show_class {
    line-height: 20%;
}
.player_show_class {
    text-align: center;
}
</style>
