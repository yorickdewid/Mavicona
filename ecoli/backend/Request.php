<?php

namespace Ecoli;

abstract class HttpMethod
{
    const Get = 0;
    const Post = 1;
    const Put = 2;
    const Delete = 3;
}

class Request
{
	/**
	 * Request method.
	 *
	 * @var HttpMethod
	 */
	private $method;
	
	/**
	 * Request uri.
	 *
	 * @var string
	 */
	private $uri;

	/**
	 * Request parameters and query string.
	 *
	 * @var Array
	 */
	private $parameters = [];

	/**
	 * Gather request method.
	 *
	 * @return void
	 */
	private function gatherMethod()
	{
		switch ($_SERVER['REQUEST_METHOD']) {
			case 'PUT':
				$method = HttpMethod::Put;
				break;
			case 'POST':
				$method = HttpMethod::Post;
				break;
			case 'GET':
				$method = HttpMethod::Get;
				break;
			case 'DELETE':
				$method = HttpMethod::Delete;
				break;
			default:
				//handle_error($request);
				break;
		}
	}

	/**
	 * Gather request URI.
	 *
	 * @return void
	 */
	private function gatherUri()
	{
		$uri = $_SERVER['PATH_INFO'];
	}

	/**
	 * Gather request query string.
	 *
	 * @return void
	 */
	private function gatherParameters()
	{
		$parameters = $_GET;
	}

	/**
	 * Capture all HTTP request properties.
	 *
	 * @return Ecoli\Request
	 */
	public static function capture()
	{
		$request = new Request;

		$request->gatherMethod();
		$request->gatherUri();
		$request->gatherParameters();

		return $request;
	}

}
