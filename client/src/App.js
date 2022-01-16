import './App.css'
import React, { useEffect, useRef, useState } from 'react'
import { Upload, Button, Input, message, Tag } from 'antd'
import { UploadOutlined } from '@ant-design/icons'
const http = require('http');

function App() {

  // ip and port can be modified
  const cppHostName = '127.0.0.1'
  const cppPort = 4000

  const [id, setID] = useState(0)
  const [username, setUsername] = useState('')

  const [messages, setMessages] = useState("")
  const [messagesList, setMessagesList] = useState([])

  const [friends, setFriends] = useState([])
  const [chattingFriend, setChattingFriend] = useState("")

  const [options, setOptions] = useState(false)
  const [listFriends, setListFriends] = useState(false)
  const [addFriend, setAddFriend] = useState(false)
  const [removeFriend, setRemoveFriend] = useState(false)
  const [chatRoom, setChatRoom] = useState(false)
  const [chatting, setChatting] = useState(false)

  const [fileLength, setFileLength] = useState(0)
  const [fileString, setFileString] = useState("")

  useEffect(() => {
    if(chatting){
      bodyRef.current.scrollTop = bodyRef.current.scrollHeight
    }
  }, [chatting])
  useEffect(() => {
    if(fileLength > 0) {
      // let j = 1
      // while(true){
      let da = `DownloadImme${id}`
      // da += reader.result[i]
      // i++; j++
      let option = {
        hostname: cppHostName,
        port: cppPort,
        method: 'POST',
        headers: {
          'Content-Type': 'text/plain',
          'Content-Length': da.length
        }
      }
      let req = http.request(option, res => {
        // console.log(`statusCode: ${res.statusCode}`)
        res.on('data', d => {
          setFileString(d.toString('utf8'))
          // console.log(d)
          // for(var i=0; i<d.length; i++){
          //   fileString += String.fromCharCode(d[i])
          // }
          // console.log(fileString)
          // console.log(fileString.length)
        })
      })
      req.on('error', error => {
        console.error(error)
      })
      req.write(da);
      req.end()
      // j = 0
      // da = ""
    }
  }, [fileLength])

  useEffect(() => {
    console.log(fileString)
    // if(fileString !== "") {
    let _option = {
      hostname: jsHostName,
      port: 5000,
      method: 'POST',
      headers: {
        'Content-Type': 'text/plain',
        'Content-Length': fileString.length
      }
    }
    let _req = http.request(_option, _res => {
      console.log(_res)
      // setFileString("")
      setFileLength(fileLength - 4045)
    })
    _req.on('error', error => {
      console.error(error)
    })
    _req.write(`${fileString}`);
    _req.end()
    // }
  }, [fileString])

  const bodyRef = useRef(null)
  const jsHostName = '127.0.0.1'
  const MAX_SIZE_OF_DATA = 3000;
  const displayStatus = (s) => {
    if (s.msg) {
      const { type, msg } = s
      const content = {
        content: msg,
        duration: 0.5
      }
      switch (type) {
        case 'success':
          message.success(content)
          break
        case 'info':
          message.info(content)
          break
        case 'danger':
        default:
          message.error(content)
          break
      }
    }
  }
  let friendString = "", messageString = ""

  return (
    <div className="App">
      {!options ?
        // login page
        <> 
          <div className="App-title">
            <h1>Login/Register</h1>
          </div>
          <Input
            placeholder="Please enter a username:"
            style={{ marginBottom: 10 }}
            onChange={(e) => setUsername(e.target.value)}
          ></Input>
          <Input.Search
            type="password"
            placeholder="Please enter a password:"
            enterButton="Continue"
            style={{ marginBottom: 10 }}
            onSearch={(passwd) => {
              if(username && passwd) {
                ////////////////////////// sendMessage(`Login {username}`)
                const data = `Login${username} ${passwd}`
                const option = {
                  hostname: cppHostName,
                  port: cppPort,
                  method: 'POST',
                  headers: {
                    'Content-Type': 'text/plain',
                    // 'Content-Length': da.length,
                  }
                }
                const req = http.request(option, res => {
                  // console.log(`statusCode: ${res.statusCode}`)
                  res.on('data', d => {
                    // console.log(d)
                    if(d[0] === 120){
                      displayStatus({
                        type: 'error',
                        msg: 'Logged in failed'
                      })
                    }
                    else {
                      setID(d);
                      displayStatus({
                        type: 'success',
                        msg: 'Logged in sucessfully'
                      })
                      setOptions(true)
                    }
                  })
                })
                req.on('error', error => {
                  console.error(error)
                })
                req.write(data)
                req.end()
              }
              else {
                displayStatus({
                  type: 'error',
                  msg: 'Please enter a username and a password.'
                })
              } 
            }}>
          </Input.Search>
        </> 
        : 
        <>
          {listFriends ?
            // list friends page
            <>
              <div className="App-title">
                <h1>All friends</h1>
              </div>
              <div>
                {friends.map((e, i) => (
                  <p className="App-message" key={i}>
                    <Tag color="blue">{e}</Tag>
                  </p>
                ))}
              </div>
              <Button type="primary" danger onClick={() => {setListFriends(false)}}>
                Leave
              </Button>
            </>
            : 
            <>
              {addFriend ? 
                // add friend page
                <>
                  <div className="App-title">
                    <h1>Add friends</h1>
                  </div>
                  <Input.Search
                    placeholder="Please enter a username:"
                    enterButton="Add"
                    style={{ marginBottom: 10 }}
                    // onChange={(e) => setUsername(e.target.value)}
                    onSearch={(msg) => {
                      if(msg) {
                          ////////////////////// sendMessage(`Add {username}`)
                          const data = `Add ${msg} ${id}`
                          const option = {
                            hostname: cppHostName,
                            port: cppPort,
                            method: 'POST',
                            headers: {
                              'Content-Type': 'text/plain',
                              // 'Content-Length': da.length
                            }
                          }
                          const req = http.request(option, res => {
                            // console.log(`statusCode: ${res.statusCode}`)
                            res.on('data', d => {
                              // console.log(d)
                              if(d[0] === 49){
                                displayStatus({
                                  type: 'error',
                                  msg: 'No user found.'
                                })
                              }
                              else if(d[0] === 48){
                                displayStatus({
                                  type: 'success',
                                  msg: 'Added successfully.'
                                })
                              }
                            })
                          })
                          req.on('error', error => {
                            console.error(error)
                          })
                          req.write(data)
                          req.end()
                      }
                      else {
                        displayStatus({
                          type: 'error',
                          msg: 'Please enter a username.'
                        })
                      } 
                    }}>
                  </Input.Search>
                  <Button type="primary" danger onClick={() => setAddFriend(false)}>
                    Leave
                  </Button>
                </>
                :
                <>
                  {removeFriend ? 
                    <>
                      <div className="App-title">
                        <h1>Remove friends</h1>
                      </div>
                      <div>
                        {friends.map((e, i) => (
                          <p className="App-message" key={i}>
                            <Button type="ghost" onClick={() => {
                              const data = `Remove ${e} ${id}`
                              const option = {
                                hostname: cppHostName,
                                port: cppPort,
                                method: 'POST',
                                headers: {
                                  'Content-Type': 'text/plain',
                                  // 'Content-Length': da.length
                                }
                              }
                              const req = http.request(option, res => {
                                // console.log(`statusCode: ${res.statusCode}`)
                                res.on('data', d => {
                                  // console.log(d)
                                  displayStatus({
                                    type: 'success',
                                    msg: 'Removed successfully.'
                                  })
                                  const index = friends.indexOf(e);
                                  const copy = [...friends]
                                  copy.splice(index, 1)
                                  setFriends(copy)
                                })
                              })
                              req.on('error', error => {
                                console.error(error)
                              })
                              req.write(data)
                              req.end()
                            }}>{e}</Button>
                          </p>
                        ))}
                      </div>
                      <Button type="primary" danger onClick={() => {
                        setRemoveFriend(false)
                      }}>
                        Leave
                      </Button>
                    </>
                    :
                    <>
                      {chatRoom ? 
                        <>
                          <div className="App-title">
                            <h1>Chat room</h1>
                          </div>
                          <div>
                            {friends.map((e, i) => (
                              <p className="App-message" key={i}>
                                <Button type="ghost" onClick={() => {
                                  const data = `Chat ${e} ${id}`
                                  const option = {
                                    hostname: cppHostName,
                                    port: cppPort,
                                    method: 'POST',
                                    headers: {
                                      'Content-Type': 'text/plain',
                                      // 'Content-Length': da.length
                                    }
                                  }
                                  const req = http.request(option, res => {
                                    // console.log(`statusCode: ${res.statusCode}`)
                                    res.on('data', d => {
                                      // console.log(d)
                                      messageString = ""
                                      for(let i=0; i<d.length; i++){
                                        messageString += String.fromCharCode(d[i])
                                      }
                                      messageString = messageString.slice(0,-1)
                                      // console.log(messageString)
                                      const copy = messageString.split('\n')
                                      // copy.splice(0, 1)
                                      // console.log(copy)
                                      setMessagesList(copy)
                                    })
                                  })
            
                                  req.on('error', error => {
                                    console.error(error)
                                  })
          
                                  req.write(data)
                                  req.end()
                                  setChattingFriend(e)
                                  setChatting(true)
                                  setChatRoom(false)
                                }}>{e}</Button>
                              </p>
                            ))}
                          </div>

                          <Button type="primary" danger onClick={() => {
                            setChatRoom(false)
                          }}>Leave</Button>
                        </>
                        :
                        <>
                          {chatting ? 
                            <>
                              <div className="App-title">
                                <h1>{chattingFriend}</h1>
                              </div>
                              <div className="App-messages" ref={bodyRef}>
                                {messagesList.map((e, i) => (
                                  e[0] === 'A' ? (
                                    <p key={i} align="right">
                                      <Tag color="#69c0ff">{e.slice(3)}</Tag>
                                    </p> 
                                  ) : ( e[0] === 'B' ? (
                                      <p key={i} align="left">
                                        <Tag color="#95de64">{e.slice(3)}</Tag>
                                      </p>
                                    ) : ( e[1] === 'A' ? (
                                      <p key={i} align="right">
                                        <Tag color="#096dd9" onClick={() => {
                                          let da = `Download${id} ${e.slice(4)}`
                                          let option = {
                                            hostname: cppHostName,
                                            port: cppPort,
                                            method: 'POST',
                                            headers: {
                                              'Content-Type': 'text/plain',
                                              'Content-Length': da.length
                                            }
                                          }
                                          let req = http.request(option, res => {
                                            // console.log(`statusCode: ${res.statusCode}`)
                                            res.on('data', d => {
                                              console.log(parseInt(d.toString('utf8')))
                                              setFileLength(parseInt(d.toString('utf8')))
                                            })
                                          })
                                          req.on('error', error => {
                                            console.error(error)
                                          })
                                          req.write(da)
                                          req.end()

                                          let _option = {
                                            hostname: jsHostName,
                                            port: 5000,
                                            method: 'POST',
                                            headers: {
                                              'Content-Type': 'text/plain',
                                              'Content-Length': e.slice(4).length
                                            }
                                          }
                                          let _req = http.request(_option, _res => {
                                            console.log(_res)
                                          })
                                          _req.on('error', error => {
                                            console.error(error)
                                          })
                                          _req.write(`FileName ${e.slice(4)}`);
                                          _req.end()
                                        }}>{e.slice(4)}</Tag>
                                      </p> 
                                      ) : (
                                        e.slice(-3) === 'png' || e.slice(-3) === 'jpg' ? (
                                          <img src="" alt=""></img>
                                        ) : (
                                          <p key={i} align="left">
                                            <Tag color="#389e0d" onClick={() => {
                                              let da = `Download${id} ${e.slice(4)}`
                                              let option = {
                                                hostname: cppHostName,
                                                port: cppPort,
                                                method: 'POST',
                                                headers: {
                                                  'Content-Type': 'text/plain',
                                                  'Content-Length': da.length
                                                }
                                              }
                                              let req = http.request(option, res => {
                                                // console.log(`statusCode: ${res.statusCode}`)
                                                res.on('data', d => {
                                                  console.log(parseInt(d.toString('utf8')))
                                                  setFileLength(parseInt(d.toString('utf8')))
                                                })
                                              })
                                              req.on('error', error => {
                                                console.error(error)
                                              })
                                              req.write(da)
                                              req.end()
    
                                              let _option = {
                                                hostname: jsHostName,
                                                port: 5000,
                                                method: 'POST',
                                                headers: {
                                                  'Content-Type': 'text/plain',
                                                  'Content-Length': e.slice(4).length
                                                }
                                              }
                                              let _req = http.request(_option, _res => {
                                                console.log(_res)
                                              })
                                              _req.on('error', error => {
                                                console.error(error)
                                              })
                                              _req.write(`FileName ${e.slice(4)}`);
                                              _req.end()
                                            }}>{e.slice(4)}</Tag>
                                          </p> 
                                        )
                                      )
                                    )
                                  )
                                ))}
                              </div>
                              <Input.Search
                                // rows={4}
                                value={messages}
                                // ref={bodyRef}
                                enterButton="Send"
                                onChange={(e) => setMessages(e.target.value)}
                                placeholder="Type a message here..."
                                onSearch={(msg) => {
                                  if (!msg) {
                                    displayStatus({
                                      type: 'error',
                                      msg: 'Please enter a message.'
                                    })
                                    return
                                  }
                                  else {
                                    const da = `Text${id} ${msg}`
                                    const option = {
                                      hostname: cppHostName,
                                      port: cppPort,
                                      method: 'POST',
                                      headers: {
                                        'Content-Type': 'text/plain',
                                        'Content-Length': da.length
                                      }
                                    }
            
                                    const req = http.request(option, res => {
                                      // console.log(`statusCode: ${res.statusCode}`)
                                      res.on('data', d => {
                                        // console.log(d)
                                      })
                                    })
              
                                    req.on('error', error => {
                                      console.error(error)
                                    })
            
                                    req.write(da)
                                    req.end()
                                    msg = "A: " + msg
                                    setMessagesList([...messagesList, msg])
                                  }
                                  setMessages('')
                                }}
                              ></Input.Search>
                              <br/>
                              <Upload showUploadList={false} beforeUpload={(file) => {
                                const reader = new FileReader();

                                reader.onload = () => {
                                  // console.log(reader.result)
                                  let da = `FileName${id} ${file.name}`
                                  let option = {
                                    hostname: cppHostName,
                                    port: cppPort,
                                    method: 'POST',
                                    headers: {
                                      'Content-Type': 'text/plain',
                                      'Content-Length': da.length
                                    }
                                  }
                                  let req = http.request(option, res => {
                                    // console.log(`statusCode: ${res.statusCode}`)
                                    res.on('data', d => {
                                      // console.log(d)
                                    })
                                  })
                                  req.on('error', error => {
                                    console.error(error)
                                  })
                                  req.write(da)
                                  req.end()
                                  

                                  let i = 0, j = 0;
                                  da = ""
                                  console.log(reader.result.length)
                                  while(i < reader.result.length){
                                    da += reader.result[i]
                                    i++; j++
                                    if(j === MAX_SIZE_OF_DATA - id.length - 9){
                                      option = {
                                        hostname: cppHostName,
                                        port: cppPort,
                                        method: 'POST',
                                        headers: {
                                          'Content-Type': 'text/plain',
                                          'Content-Length': da.length
                                        }
                                      }
                                      req = http.request(option, res => {
                                        // console.log(`statusCode: ${res.statusCode}`)
                                        res.on('data', d => {
                                          // console.log(d)
                                        })
                                      })
                                      req.on('error', error => {
                                        console.error(error)
                                      })
                                      req.write(`FileImme${id} ${da}`);
                                      console.log(da)
                                      req.end()
                                      j = 0
                                      da = ""
                                    }
                                  }

                                  console.log(da.length)

                                  let len = da.length
                                  da += '\0'.repeat(MAX_SIZE_OF_DATA - da.length - 12 - id.length - `${da.length}`.length)
                                  da = `FileFinish${id} ${len} ${da}`
                                  option = {
                                    hostname: cppHostName,
                                    port: cppPort,
                                    method: 'POST',
                                    headers: {
                                      'Content-Type': 'text/plain',
                                      'Content-Length': da.length
                                    }
                                  }
                                  req = http.request(option, res => {
                                    // console.log(`statusCode: ${res.statusCode}`)
                                    res.on('data', d => {
                                      // console.log(d)
                                    })
                                  })
                                  req.on('error', error => {
                                    console.error(error)
                                  })
                                  req.write(da)
                                  displayStatus({
                                    type: 'success',
                                    msg: 'Uploaded successfully.'
                                  })
                                  req.end()
                                  setMessagesList([...messagesList, `FA: ${file.name}`])
                                };
                                reader.readAsBinaryString(file);

                                // Prevent upload
                                return false;
                              }}>
                                <Button icon={<UploadOutlined />}>Upload a file or an image</Button>
                              </Upload>
                              <br/>
                              <Button type="primary" danger onClick={() => {
                                setChatting(false)
                                setChatRoom(true)
                              }}>Leave</Button>
                            </>
                            :
                            <>
                              <div className="App-title">
                                <h1>Welcome, {username}!</h1>
                              </div>
                              <div className="App-title">
                                <h2>Choose an option to continue...</h2>
                              </div>
                              <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
                                /////////////////////sendMessage('List friends')
                                // http.onload = () => console.log(http.responseText)
                                // http.send('List friends')
                                //////////////////////////
                                // const da = JSON.stringify({
                                //   todo: 'Buy the milk'
                                // })
                                const da = "List friends"
                                const option = {
                                  hostname: cppHostName,
                                  port: cppPort,
                                  method: 'POST',
                                  headers: {
                                    'Content-Type': 'text/plain',
                                    'Content-Length': da.length,
                                  }
                                }

                                const req = http.request(option, res => {
                                  // console.log(`statusCode: ${res.statusCode}`)
                                  res.on('data', d => {
                                    // console.log(d)
                                    friendString = ""
                                    for(var i=0; i<d.length; i++){
                                      friendString += String.fromCharCode(d[i])
                                    }
                                    friendString = friendString.slice(0,-1)
                                    // console.log(friendString)
                                    setFriends(friendString.split('\n'))
                                  })
                                })

                                req.on('error', error => {
                                  console.error(error)
                                })

                                req.write(`List friends ${id}`)
                                req.end()
                                /////////////////////////////
                                setListFriends(true)
                              }}>
                                List all friends
                              </Button>
                              <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
                                setAddFriend(true)
                              }}>
                                Add friends
                              </Button>
                              <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
                                const da = "List friends"
                                const option = {
                                  hostname: cppHostName,
                                  port: cppPort,
                                  method: 'POST',
                                  headers: {
                                    'Content-Type': 'text/plain',
                                    'Content-Length': da.length,
                                  }
                                }

                                const req = http.request(option, res => {
                                  // console.log(`statusCode: ${res.statusCode}`)
                                  res.on('data', d => {
                                    // console.log(d)
                                    friendString = ""
                                    for(var i=0; i<d.length; i++){
                                      friendString += String.fromCharCode(d[i])
                                    }
                                    friendString = friendString.slice(0,-1)
                                    // console.log(friendString)
                                    setFriends(friendString.split('\n'))
                                  })
                                })

                                req.on('error', error => {
                                  console.error(error)
                                })

                                req.write(`List friends ${id}`)
                                req.end()
                                setRemoveFriend(true)
                              }}>
                                Remove friends
                              </Button>
                              <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
                                const da = "List friends"
                                const option = {
                                  hostname: cppHostName,
                                  port: cppPort,
                                  method: 'POST',
                                  headers: {
                                    'Content-Type': 'text/plain',
                                    'Content-Length': da.length,
                                  }
                                }

                                const req = http.request(option, res => {
                                  // console.log(`statusCode: ${res.statusCode}`)
                                  res.on('data', d => {
                                    // console.log(d)
                                    friendString = ""
                                    for(var i=0; i<d.length; i++){
                                      friendString += String.fromCharCode(d[i])
                                    }
                                    friendString = friendString.slice(0,-1)
                                    // console.log(friendString)
                                    setFriends(friendString.split('\n'))
                                  })
                                })

                                req.on('error', error => {
                                  console.error(error)
                                })

                                req.write(`List friends ${id}`)
                                req.end()
                                setChatRoom(true)
                              }}>
                                Chat with
                              </Button>
                            </>
                          }
                        </>
                      }
                    </>
                  }
                </>
              }
            </>
          }
        </>
      }
    </div>
  )
}

export default App
