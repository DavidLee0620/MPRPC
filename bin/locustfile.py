from locust import User, task, constant, events
import subprocess
import time
import logging
import os
from concurrent.futures import ProcessPoolExecutor
# 配置日志
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class SimpleConsumerUser(User):
    consumer_path = "./consumer"
    config_path = "test.conf"  # 确保这是正确的配置文件路径
    wait_time = constant(0)  

    @task
    def run_consumer(self):
        start_time = time.perf_counter()  # 使用高精度计时器
        command = [self.consumer_path, "-i", self.config_path]
        success = False  # 初始化成功标志为False
        stdout = b''  # 初始化stdout为空
        
        try:
            # 检查consumer_path和config_path是否存在
            if not os.path.exists(self.consumer_path):
                logger.error(f"consumer程序文件不存在：{self.consumer_path}")
                raise FileNotFoundError(f"consumer程序文件不存在：{self.consumer_path}")
            
            if not os.path.exists(self.config_path):
                logger.error(f"配置文件不存在：{self.config_path}")
                raise FileNotFoundError(f"配置文件不存在：{self.config_path}")
            
            # 使用with自动管理进程资源
            with subprocess.Popen(
                command,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
            ) as process:
                
                stdout, stderr = process.communicate()
                success = process.returncode == 0
                
                # 添加详细错误日志
                if not success:
                    error_msg = stderr.decode('utf-8', errors='ignore')[:500]  # 限制长度
                    logger.error(f"失败[代码:{process.returncode}]: {error_msg}")

        except subprocess.TimeoutExpired as e:
            logger.error(f"进程执行超时：{e}")
            # 如果是超时异常，也需要记录日志并设置success为False
            success = False
        except FileNotFoundError as e:
            logger.error(f"文件未找到：{e}")
            success = False
        except Exception as e:
            logger.exception(f"系统异常: {e}")  # 记录完整堆栈
            success = False
        
        finally:
            response_time = int((time.perf_counter() - start_time) * 1000)

            
            # 记录请求事件
            exception = None
            if not success:
                exception = Exception("请求失败")
            
            self.environment.events.request.fire(
                request_type="RPC",
                name="consumer_rpc",
                response_time=response_time,
                response_length=len(stdout),
                exception=exception,
                context={"command": command}
            )

# 添加自定义事件监听器
@events.init.add_listener
def on_locust_init(environment, **kwargs):
    logger.info("Locust初始化完成")

@events.quitting.add_listener
def on_locust_quit(environment, **kwargs):
    logger.info("Locust正在退出")
    # 添加清理逻辑，确保所有子进程被终止
    for user in environment.users:
        if hasattr(user, 'process') and user.process:
            user.process.terminate()
            user.process.wait()