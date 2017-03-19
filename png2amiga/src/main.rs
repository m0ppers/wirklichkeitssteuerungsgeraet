extern crate png;

use std::env;
use std::fs::{File, OpenOptions};
use png::{ColorType, Decoder};
use std::collections::HashMap;
use std::error::Error;
use std::path::Path;
use std::io::Write;

fn main() {
    let args: Vec<_> = env::args().collect();
    if args.len() != 3 {
        panic!("Must provide exactly 2 parameters. <infile> and <outfile>");
    }
    let rfilename = args[1].as_str();
    let wfilename = args[2].as_str();
    
    let decoder1 = Decoder::new(File::open(rfilename).unwrap());
    let (_, reader) = decoder1.read_info().unwrap();

    let info = &reader.info();
    
    if info.color_type != ColorType::Indexed {
        panic!("Need an indexed png! Type is: {:?}", info.color_type);
    }
    
    let color_map = match info.palette {
        Some(ref palette) => {
            let mut color_map = HashMap::new();
            let mut color_key = 0u8;
            for (key, color) in palette.chunks(3).enumerate() {
                if let Some(ref trns) = info.trns {
                    if trns.iter().any(|&index| index == key as u8) {
                        continue;
                    }
                }
                color_map.insert(color.clone(), color_key);
                color_key += 1;
            }
            color_map
        },
        None => {
            panic!("Couldn't find palette!");
        }
    };
    let num_colors = color_map.len() as u8;

    let pixel_length = 3u8 + info.trns.is_some() as u8;

    let bits:u8 = ((num_colors + info.trns.is_some() as u8) as f64).log(2.0).ceil() as u8;
    let decoder = Decoder::new(File::open(rfilename).unwrap());
    let (output_info, mut reader) = decoder.read_info().unwrap();

    let plane_length = output_info.width as usize * output_info.height as usize / 8;
    let mut buffer: Vec<u8> = vec![0u8; plane_length * bits as usize];
    
    let mut img_data = vec![0; output_info.buffer_size()];

    match reader.next_frame(&mut img_data) {
        Ok(_) => {
            for (pixel_index, full_pixel) in img_data.chunks(pixel_length as usize).enumerate() {
                if info.trns.is_some() {
                    if full_pixel[3] == 0 {
                        continue;
                    }
                }
                let pixel: &[u8] = &[full_pixel[0], full_pixel[1],full_pixel[2]];
                let pixel_offset = pixel_index % 8;
                println!("{:?}", pixel);
                let color_opt = color_map.get(&pixel);
                if let Some(color) = color_opt {
                    for plane in 0u8..bits {
                        if color & 2u8.pow(plane as u32) > 0 {
                            buffer[pixel_index/8 as usize + plane_length * plane as usize] |= 1 << (7 - pixel_offset);
                        }
                        //println!("{} {} {}", group_index + plane_length * plane as usize, color, buffer[group_index + plane_length * plane as usize]);
                    }
                }
            }
        },
        Err(err) => {
            panic!("Got an error: {}", err);
        }
    };

    let wpath = Path::new(wfilename);
    let mut wfile = match OpenOptions::new()
                             .create(true)
                             .truncate(true)
                             .write(true)
                             .open(&wpath) {
        // The `description` method of `io::Error` returns a string that
        // describes the error
        Err(why) => panic!("couldn't open {}: {}", wfilename,
                                                   why.description()),
        Ok(file) => file,
    };
    let _ = wfile.write(buffer.as_slice());
}