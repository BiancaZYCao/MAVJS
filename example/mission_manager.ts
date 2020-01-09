import { Mavsdk, System, Telemetry, MissionItem, Mission, Action } from "../lib/addon";

function usage(bin_name: string): void
{
    console.log(`Usage : ${bin_name} <connection_url>
                Connection URL format should be :
                For TCP : tcp://[server_host][:server_port]
                For UDP : udp://[bind_host][:bind_port]
                For Serial : serial:///path/to/serial/dev[:baudrate]
                For example, to connect to the simulator use URL: udp://:14540`);
}

function sleep(millis: number) {
    return new Promise(resolve => setTimeout(resolve, millis));
}

function make_mission_item(
    latitude_deg: number,
    longitude_deg: number,
    relative_altitude_m: number,
    speed_m_s: number,
    is_fly_through: boolean,
    gimbal_pitch_deg: number,
    gimbal_yaw_deg: number,
    camera_action: MissionItem.CameraAction)
{
    let new_item = new MissionItem.Init();
    new_item.set_position(latitude_deg, longitude_deg);
    new_item.set_relative_altitude(relative_altitude_m);
    new_item.set_speed(speed_m_s);
    new_item.set_fly_through(is_fly_through);
    new_item.set_gimbal_pitch_and_yaw(gimbal_pitch_deg, gimbal_yaw_deg);
    new_item.set_camera_action(camera_action);
    return new_item.to_object();
}


let main = async () => {
    
    const argc = process.argv.length - 2;

    let mavsdk = new Mavsdk.Init();

    let discovered_system: boolean = false;


    let connection_result: Mavsdk.ConnectionResult;
    if (argc == 1) {
        const connection_url: string = process.argv[2];
        connection_result = mavsdk.add_any_connection(connection_url);
    } else {
        usage(process.argv[1]);
        process.exit()
    }

    if (connection_result != Mavsdk.ConnectionResult.SUCCESS) {
        console.log(`Connection failed: ${connection_result.toString()}`);
        process.exit()
    }

    console.log("Waiting to discover system...");
    {
        let prom = function() {
            return new Promise((resolve) => {
                    mavsdk.register_on_discover((uuid) => {
                    console.log(`Discovered system with UUID: ${uuid}`);
                    resolve();
                });
            });
        }

        await prom();
    }
    
    mavsdk.register_on_timeout((uuid) => {
        console.log(`System with UUID timed out: ${uuid}`);
        process.exit();
    })

    let system = mavsdk.system();
    
    let telemetry = system.telemetry();
    let action = system.action();
    let mission = system.mission();

    // Check if vehicle is ready to arm
    while (telemetry.health_all_ok() != true) {
        console.log("Vehicle is getting ready to arm")
        await sleep(1000);
    }

    console.log("System ready");
    console.log("Creating and uploading mission");

    let mission_items = [];

    let i = 0;

    /*while(i < 100) {
    mission_items.push(make_mission_item(
        47.398170327054473,
        8.5456490218639658,
        10.0,
        5.0,
        false,
        20.0,
        60.0,
        MissionItem.CameraAction.NONE));

    mission_items.push(make_mission_item(
        47.398241338125118,
        8.5455360114574432,
        10.0,
        2.0,
        true,
        0.0,
        -60.0,
        MissionItem.CameraAction.TAKE_PHOTO));

    mission_items.push(make_mission_item(
        47.398139363821485,
        8.5453846156597137,
        10.0,
        5.0,
        true,
        -45.0,
        0.0,
        MissionItem.CameraAction.START_VIDEO));

    mission_items.push(make_mission_item(
        47.398058617228855,
        8.5454618036746979,
        10.0,
        2.0,
        false,
        -90.0,
        30.0,
        MissionItem.CameraAction.STOP_VIDEO));

    mission_items.push(make_mission_item(
        47.398100366082858,
        8.5456969141960144,
        10.0,
        5.0,
        false,
        -45.0,
        -30.0,
        MissionItem.CameraAction.START_PHOTO_INTERVAL));

    mission_items.push(make_mission_item(
        47.398001890458097,
        8.5455576181411743,
        10.0,
        5.0,
        false,
        0.0,
        0.0,
        MissionItem.CameraAction.STOP_PHOTO_INTERVAL));
    i = i +1;
    }*/
 

    //console.log("Uploading mission...");
    //await mission.upload_mission_async(mission_items);

    /*mission.download_mission_async().then((result) => {
        console.log(result.mission_items.length)
        if (result.result != Mission.Result.SUCCESS) {
            console.log(`Mission download failed ${result.result}, exiting.`)
            process.exit();
        }
    })
   
    await sleep(200);
    
    console.log("Cancelling mission...");
    mission.download_mission_cancel();
    console.log("ok");*/
   
    //console.log("Mission uploaded.");
    console.log(mission.get_return_to_launch_after_mission());
    console.log(mission.set_return_to_launch_after_mission(true));
    console.log(mission.get_return_to_launch_after_mission());
    process.exit();
}

main();