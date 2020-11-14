<template>
<div class="home">
    <div v-if="!is_login">
        <el-row :gutter="20">
            <el-col :span="12">
                <el-button type="success" round @click="nav_to_wechat_login">
                    微信登陆
                </el-button>
            </el-col>
            <el-col :span="12">
                <el-button type="primary" round @click="nav_to_qq_login">
                    <img src='data:img/jpg;base64,iVBORw0KGgoAAAANSUhEUgAAAKoAAAAgCAYAAAH1WtOqAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJ
bWFnZVJlYWR5ccllPAAAA2RpVFh0WE1MOmNvbS5hZG9iZS54bXAAAAAAADw/eHBhY2tldCBiZWdp
bj0i77u/IiBpZD0iVzVNME1wQ2VoaUh6cmVTek5UY3prYzlkIj8+IDx4OnhtcG1ldGEgeG1sbnM6
eD0iYWRvYmU6bnM6bWV0YS8iIHg6eG1wdGs9IkFkb2JlIFhNUCBDb3JlIDUuMC1jMDYwIDYxLjEz
NDc3NywgMjAxMC8wMi8xMi0xNzozMjowMCAgICAgICAgIj4gPHJkZjpSREYgeG1sbnM6cmRmPSJo
dHRwOi8vd3d3LnczLm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4gPHJkZjpEZXNjcmlw
dGlvbiByZGY6YWJvdXQ9IiIgeG1sbnM6eG1wTU09Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEu
MC9tbS8iIHhtbG5zOnN0UmVmPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvc1R5cGUvUmVz
b3VyY2VSZWYjIiB4bWxuczp4bXA9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC8iIHhtcE1N
Ok9yaWdpbmFsRG9jdW1lbnRJRD0ieG1wLmRpZDpDMjhDMzhDMzBDNTFFMDExQkQzMkRDMDMwQjJG
RTBBQSIgeG1wTU06RG9jdW1lbnRJRD0ieG1wLmRpZDo0NEREMDZDNDY1QjExMUUwQTA4OEQ5NEU2
QUY3QjNDMCIgeG1wTU06SW5zdGFuY2VJRD0ieG1wLmlpZDo0NEREMDZDMzY1QjExMUUwQTA4OEQ5
NEU2QUY3QjNDMCIgeG1wOkNyZWF0b3JUb29sPSJBZG9iZSBQaG90b3Nob3AgQ1M1IFdpbmRvd3Mi
PiA8eG1wTU06RGVyaXZlZEZyb20gc3RSZWY6aW5zdGFuY2VJRD0ieG1wLmlpZDpFRTU3NzlBNjBE
NUNFMDExQUEzREI4REM0RTY2RDJGNSIgc3RSZWY6ZG9jdW1lbnRJRD0ieG1wLmRpZDpDMjhDMzhD
MzBDNTFFMDExQkQzMkRDMDMwQjJGRTBBQSIvPiA8L3JkZjpEZXNjcmlwdGlvbj4gPC9yZGY6UkRG
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' />
                </el-button>
            </el-col>
        </el-row>
    </div>
    <div v-else>
        <el-container>
            <el-header>
                <el-row :gutter="20">
                    <el-col :span="12">
                        <el-avatar :src="user_logo" :size="50"></el-avatar>
                    </el-col>
                    <el-col :span="12">
                        {{user_name}}
                    </el-col>
                </el-row>
            </el-header>
            <el-main>
                <el-row :gutter="20">
                    <el-col :span="12">
                        <i class="el-icon-money">当前金额</i>
                    </el-col>
                    <el-col :span="12">
                        {{user_cash}}
                    </el-col>
                </el-row>
                <el-row :gutter="20">
                    <el-col :span="6">
                        充值金额
                    </el-col>
                    <el-col :span="12">
                        <el-input-number v-model="added_cash" :step="1000" :max="50000"></el-input-number>
                    </el-col>
                    <el-col :span="6">
                        <el-button type="primary" @click="add_cash">加钱</el-button>
                    </el-col>
                </el-row>
                <el-row :gutter="20">
                    <el-col :span="8">
                        <el-button type="primary" @click="create_table">创建牌桌</el-button>
                    </el-col>
                    <el-col :span="8">
                        <el-button type="primary" @click="enter_random">随机入桌</el-button>
                    </el-col>
                    <el-col :span="8">
                        <el-button type="primary" @click="enter_table">按号入桌</el-button>
                    </el-col>
                </el-row>
                <el-button type="success" v-if="belong_to_table != -1" @click="re_con_table">重连到{{belong_to_table}}号桌</el-button>
            </el-main>
            <el-footer>
                <el-button type="danger" @click="logoff">退出登录</el-button>
            </el-footer>
        </el-container>
    </div>
    <el-link type="info" href="http://beian.miit.gov.cn" id="beian">京ICP备2020039126号-1</el-link>
</div>
</template>

<script>
// @ is an alias to /src
// import HelloWorld from '@/components/HelloWorld.vue'
import {
    Base64
} from 'js-base64'

export default {
    name: 'Home',
    data: function () {
        return {
            user_name: '',
            user_logo: '',
            user_cash: '',
            is_login: false,
            added_cash: 1000,
            belong_to_table: -1,
        };
    },
    methods: {
        nav_to_wechat_login: function () {
            window.location.href = "https://open.weixin.qq.com/connect/qrconnect?appid=wx987b51617d4be3ae&redirect_uri=http%3a%2f%2fwww.d8sis.cn%2fwechatlogin&response_type=code&scope=snsapi_login&state=STATE#wechat_redirect";
        },
        nav_to_qq_login: function () {
            QC.Login.showPopup({
                appId: "101912803",
                redirectURI: "http://www.d8sis.cn/qqlogin"
            });
        },
        get_cur_user_info: function () {
            var vue_this = this;
            var ssid = this.$cookies.get('ssid');
            this.$axios.get('/game_rest/userinfo/' + ssid).then(function (resp) {
                if (resp.data.result.user_name.length > 0) {
                    Base64.extendString();
                    vue_this.user_name = resp.data.result.user_name.fromBase64();
                    vue_this.user_logo = resp.data.result.user_logo;
                    vue_this.user_cash = resp.data.result.user_cash;
                    vue_this.is_login = true;
                    vue_this.belong_to_table = resp.data.result.table_no;
                }
                console.log(vue_this);
            }).catch(function (err) {
                console.log(err);
            });
        },
        logoff: function () {
            var ssid = this.$cookies.get('ssid');
            this.$axios.get('/game_rest/logoff/' + ssid).then(function (resp) {
                console.log(resp);
                window.location.replace('/');
            }).catch(function (err) {
                console.log(err);
            });
        },
        add_cash: function () {
            var vue_this = this;
            var ssid = this.$cookies.get('ssid');
            this.$axios.post('/game_rest/add_cash', {
                text: {
                    ssid: ssid,
                    cash: this.added_cash
                }
            }).then(function (resp) {
                if (resp.data.result == 'success') {
                    vue_this.get_cur_user_info();
                    vue_this.$notify({
                        title: '加钱成功',
                        message: '',
                        type: 'success'
                    });
                } else {
                    vue_this.$notify({
                        title: '加钱失败',
                        message: '',
                        type: 'error'
                    });
                }
            }).catch(function (err) {
                console.log(err);
            });
        },
        create_table: function () {
            var vue_this = this;
            this.$axios.get('/game_rest/create_table').then(function (resp) {
                if ("success" == resp.data.result.created) {
                    console.log("table no is" + resp.data.result.table_no);
                    vue_this.$router.push({
                        path: '/table/' + resp.data.result.table_no
                    });
                }
            }).catch(function (err) {
                console.log(err);
            });
        },
        enter_random: function () {

        },
        enter_table: function () {

        },
        re_con_table: function () {
            this.$router.push({
                path: '/table/' + this.belong_to_table
            });
        }
    },
    beforeMount: function () {
        this.get_cur_user_info();
    }
}
</script>

<style lang="scss" scoped>
.clearfix:before,
.clearfix:after {
    display: table;
    content: "";
}

.clearfix:after {
    clear: both
}

.el-row {
    margin-bottom: 20px;

    &:last-child {
        margin-bottom: 0;
    }

}

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

#beian {
    position: absolute;
    bottom: 0px;
    left: 0px;
}
</style>
