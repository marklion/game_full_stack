<template>
<div class="each_player">
    <el-card>
        <div>
            <p>{{seat_no}}</p>
        </div>
        <el-row :gutter="20">
            <el-col :span="12">
                <el-avatar :src="player_info.logo" :size="50"></el-avatar>
            </el-col>
            <el-col :span="12">
                {{player_info.name}}
            </el-col>
        </el-row>
        <el-row :gutter="20">
            <el-col :span="12">
                共{{player_info.total_cash}}
            </el-col>
            <el-col :span="12">
                下注{{player_info.bat_cash}}
            </el-col>
        </el-row>
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
            logo:"",
            total_cash: 0,
            bat_cash: 0,
        },
        sit_down_proc_in: Function,
        show_sit_down: Boolean,
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
            }
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
</style>
